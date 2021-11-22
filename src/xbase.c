#include "xbase.h"

#include "main.h"

#include <stdlib.h>
#include <string.h>

/* Startup Error handler to check if another
 * window manager is already running. */
static int x_error_wm_check(Display*, XErrorEvent*);

Xbase xbase_new(
	const char *const program_title,
	const XErrorHandler x_error_handler
) {
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

	if (!(xbase->x_error = XSetErrorHandler(x_error_wm_check))) {
		fatal("no X error handler is given");
	}

	// This causes an error if some other window manager is running
	XSelectInput(
		xbase->x_display,
		DefaultRootWindow(xbase->x_display),
		SubstructureRedirectMask
	);
	XSync(xbase->x_display, False);
	XSetErrorHandler(x_error_handler);
	XSync(xbase->x_display, False);

	return xbase;
}

void xbase_delete(const Xbase xbase)
{
	// TODO: maybe we should assert here
	if (xbase == NULL) return;

	XCloseDisplay(xbase->x_display);
	free(xbase);
}

int x_error_wm_check(
	__attribute__((unused)) Display *const x_display,
	__attribute__((unused)) XErrorEvent *const x_error_event
) {
	fatal("another window manager is already running");
}
