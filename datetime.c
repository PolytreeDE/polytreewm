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

static pthread_mutex_t mutex;
static bool running = false;

static Display *display = None;
static Atoms atoms = NULL;

static pthread_t thread;
static bool thread_created = false;

static char buffer[BUFFER_SIZE];

void datetime_lock()
{
	pthread_mutex_lock(&mutex);
}

void datetime_unlock()
{
	pthread_mutex_unlock(&mutex);
}

const char *datetime_get()
{
	return buffer;
}

bool datetime_start()
{
	datetime_lock();

	if (running) return false;

	running = true;
	strcpy(buffer, default_text);

	if ((display = XOpenDisplay(NULL)) == NULL) return false;
	if ((atoms = atoms_create(display)) == NULL) return false;

	thread_created = pthread_create(&thread, NULL, run, NULL) == 0;

	datetime_unlock();

	return thread_created;
}

void datetime_stop()
{
	datetime_lock();

	if (!running) return;

	running = false;
	if (thread_created) pthread_join(thread, NULL);
	if (atoms != NULL) atoms_destroy(atoms);
	if (display != None) XCloseDisplay(display);

	datetime_unlock();
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
		event.xclient.display = display;
		event.xclient.window = DefaultRootWindow(display);
		event.xclient.message_type = atoms->netatom[NetDateTime];
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
