#include "status.h"

#include "atoms.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>

#define EXTERNAL_BUFFER_SIZE 256
#define DATETIME_BUFFER_SIZE 32

#define BUFFER_SIZE (EXTERNAL_BUFFER_SIZE + 3 + DATETIME_BUFFER_SIZE)

static void update();

static pthread_mutex_t mutex;
static bool running = false;

static Display *display = None;
static Atoms atoms = NULL;

static char buffer[BUFFER_SIZE];
static char external_buffer[EXTERNAL_BUFFER_SIZE];
static char datetime_buffer[DATETIME_BUFFER_SIZE];

void status_lock()
{
	pthread_mutex_lock(&mutex);
}

void status_unlock()
{
	pthread_mutex_unlock(&mutex);
}

const char *status_get()
{
	return buffer;
}

bool status_start()
{
	status_lock();

	if (running) return false;

	running = true;

	memset(buffer, 0, sizeof(buffer));
	memset(external_buffer, 0, sizeof(external_buffer));
	memset(datetime_buffer, 0, sizeof(datetime_buffer));

	if ((display = XOpenDisplay(NULL)) == NULL) return false;
	if ((atoms = atoms_create(display)) == NULL) return false;

	status_unlock();

	return true;
}

void status_stop()
{
	status_lock();

	if (!running) return;

	running = false;
	if (atoms != NULL) atoms_destroy(atoms);
	if (display != None) XCloseDisplay(display);

	status_unlock();
}

void status_set_external(const char *const text)
{
	status_lock();

	strncpy(external_buffer, text, EXTERNAL_BUFFER_SIZE);
	external_buffer[EXTERNAL_BUFFER_SIZE - 1] = '\0';
	update();

	status_unlock();
}

void status_set_datetime(const char *const text)
{
	status_lock();

	strncpy(datetime_buffer, text, DATETIME_BUFFER_SIZE);
	datetime_buffer[DATETIME_BUFFER_SIZE - 1] = '\0';
	update();

	status_unlock();
}

static void update()
{
	sprintf(buffer, "%s | %s", external_buffer, datetime_buffer);

	XEvent event;
	memset(&event, 0, sizeof(event));
	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.display = display;
	event.xclient.window = DefaultRootWindow(display);
	event.xclient.message_type = atoms->netatom[NetStatusUpdate];
	event.xclient.format = 32;

	XSendEvent(
		display,
		event.xclient.window,
		False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		&event
	);

	XFlush(display);
}
