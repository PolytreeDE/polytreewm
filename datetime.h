#ifndef _DATETIME_H
#define _DATETIME_H

#include <stdbool.h>

bool datetime_start();
void datetime_stop();
void datetime_lock();
void datetime_unlock();
const char *datetime_get();

#endif // _DATETIME_H
