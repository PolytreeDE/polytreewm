#ifndef _CONFIG_BUTTONS_H
#define _CONFIG_BUTTONS_H

#include "common.h"

#include <stddef.h>

typedef struct {
	unsigned int click;
	unsigned int mask;
	unsigned int button;
	void (*func)(const Arg *arg);
	const Arg arg;
} Button;

enum { ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

extern const Button buttons[];

size_t buttons_count();

#endif // _CONFIG_BUTTONS_H
