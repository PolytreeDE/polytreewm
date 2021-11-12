#include "datetime.h"

#include "atoms.h"

#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define BUFFER_SIZE 32

static void *run(void *vargp);

static const char *const default_text = "date & time";

static Display *display = None;
static AtomValues atom_values = NULL;
static bool running = true;
static pthread_t thread;
static pthread_mutex_t mutex;
static char buffer[BUFFER_SIZE];

bool datetime_start()
{
	strcpy(buffer, default_text);

	if ((display = XOpenDisplay(NULL)) == NULL) {
		return false;
	}

	atom_values = atoms_create(display);

	pthread_create(&thread, NULL, run, NULL);

	return true;
}

void datetime_stop()
{
	running = false;
	atoms_destroy(atom_values);
	XCloseDisplay(display);
	pthread_join(thread, NULL);
}

void *run(void *vargp)
{
	while (running) {
		time_t raw_time;
		time(&raw_time);

		const struct tm *const time_info = localtime(&raw_time);

		datetime_lock();
		strftime(buffer, sizeof(buffer), "%a, %e %b %Y, %H:%M:%S", time_info);
		datetime_unlock();

		XEvent event;
		memset(&event, 0, sizeof(event));
		event.xclient.type = ClientMessage;
		event.xclient.serial = 0;
		event.xclient.send_event = True;
		event.xclient.display = display; // TODO: was undefined in xclimsg
		event.xclient.window = DefaultRootWindow(display);
		event.xclient.message_type = atom_values->netatom[NetDateTime];
		event.xclient.format = 32;
		XSendEvent(
			display,
			event.xclient.window,
			False,
			SubstructureRedirectMask | SubstructureNotifyMask,
			&event
		);
		XFlush(display);

		sleep(1);
	}

	return NULL;
}

void datetime_lock()
{
	pthread_mutex_lock(&mutex);
}

const char *datetime_get()
{
	return buffer;
}

void datetime_unlock()
{
	pthread_mutex_unlock(&mutex);
}
