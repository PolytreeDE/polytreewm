#include "xbase.h"

#include "main.h"

#include <stdlib.h>
#include <string.h>

Xbase xbase_new(const char *const program_title)
{
	if (!XSupportsLocale()) warning("no locale support in X");

	Xbase xbase = malloc(sizeof(struct Xbase));
	if (!xbase) fatal_perror("cannot allocate xbase");

	if (!(xbase->program_title = strdup(program_title))) {
		fatal_perror("no program title is given");
	}

	if (!(xbase->x_display = XOpenDisplay(NULL))) {
		fatal("cannot open X display");
	}

	xbase->x_screen = DefaultScreen(xbase->x_display);
	xbase->x_root = RootWindow(xbase->x_display, xbase->x_screen);
	xbase->screen_sizes = sizes_create_from_args(
		DisplayWidth(xbase->x_display, xbase->x_screen),
		DisplayHeight(xbase->x_display, xbase->x_screen)
	);

	return xbase;
}

void xbase_delete(const Xbase xbase)
{
	// TODO: maybe we should assert here
	if (xbase == NULL) return;

	XCloseDisplay(xbase->x_display);
	free(xbase);
}
