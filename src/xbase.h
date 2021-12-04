#ifndef _XBASE_H
#define _XBASE_H

#include "atoms.h"
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
	int x_root;

	struct Sizes screen_sizes;

	int (*x_error)(Display*, XErrorEvent*);

	Atoms atoms;
} *Xbase;

Xbase xbase_new(const char *program_title, XErrorHandler x_error_handler);
void xbase_delete(Xbase xbase);

#endif // _XBASE_H
