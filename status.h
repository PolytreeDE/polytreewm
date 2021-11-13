#ifndef _STATUS_H
#define _STATUS_H

#include <stdbool.h>

bool status_start();
void status_stop();
void status_lock();
void status_unlock();
const char *status_get();

void status_set_external(const char *text);
void status_set_datetime(const char *text);

#endif // _STATUS_H
