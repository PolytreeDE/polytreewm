#ifndef _DATETIME_H
#define _DATETIME_H

#include <stdbool.h>

bool datetime_start();
void datetime_stop();

void datetime_lock();
const char *datetime_get();
void datetime_unlock();

#endif // _DATETIME_H
