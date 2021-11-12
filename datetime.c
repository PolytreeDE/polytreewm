#include "datetime.h"

#include "atoms.h"
#include "status.h"

#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 32

static void datetime_lock();
static void datetime_unlock();
static void *run(void *vargp);

static const char *const default_text = "date & time";

static pthread_mutex_t mutex;
static bool running = false;

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

bool datetime_start()
{
	datetime_lock();

	if (running) return false;

	running = true;
	strcpy(buffer, default_text);

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

		status_set_datetime(buffer);

		sleep(1);
	}

	return NULL;
}
