#include "cursors.h"

#include <X11/cursorfont.h>

const Cursor cursors[CURSORS_COUNT] = { 0, 0, 0 };

void cursors_create(Display *const display)
{
	Cursor *const cursors_ptr = (Cursor*)cursors;
	cursors_ptr[CURSOR_NORMAL] = XCreateFontCursor(display, XC_left_ptr);
	cursors_ptr[CURSOR_RESIZE] = XCreateFontCursor(display, XC_sizing);
	cursors_ptr[CURSOR_MOVE]   = XCreateFontCursor(display, XC_fleur);
}

void cursors_destroy(Display *const display)
{
	Cursor *const cursors_ptr = (Cursor*)cursors;
	XFreeCursor(display, cursors_ptr[CURSOR_NORMAL]);
	XFreeCursor(display, cursors_ptr[CURSOR_RESIZE]);
	XFreeCursor(display, cursors_ptr[CURSOR_MOVE]);
}
