#ifndef _CONFIG_KEYS_H
#define _CONFIG_KEYS_H

#include "common.h"

#include <stddef.h>

#include <X11/Xlib.h>

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

extern const Key keys[];

size_t keys_count();

#endif // _CONFIG_KEYS_H
