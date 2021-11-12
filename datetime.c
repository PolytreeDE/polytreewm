#include "datetime.h"

#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 32

static void *run(void *vargp);

static const char *const default_text = "date & time";

static bool running = true;
static pthread_t thread;
static pthread_mutex_t mutex;
static char buffer[BUFFER_SIZE];

void datetime_start()
{
	strcpy(buffer, default_text);
	pthread_create(&thread, NULL, run, NULL);
}

void datetime_stop()
{
	running = false;
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
