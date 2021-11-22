#ifndef _XBASE_H
#define _XBASE_H

#include "state.h"

#include <stddef.h>
#include <X11/Xutil.h>

#define XBASE_DELETE(xbase) { \
	xbase_delete(xbase);      \
	xbase = NULL;             \
}

typedef struct Xbase {
	const char *program_title;
	Display *x_display;
	int x_screen;
	struct Sizes screen_sizes;
	int x_root;
} *Xbase;

Xbase xbase_new(const char *program_title);
void xbase_delete(Xbase xbase);

#endif // _XBASE_H
