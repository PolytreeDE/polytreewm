#ifndef _CONFIG_CURSORS_H
#define _CONFIG_CURSORS_H

#include <X11/Xlib.h>

#define CURSOR_NORMAL 0
#define CURSOR_RESIZE 1
#define CURSOR_MOVE   2

#define CURSORS_COUNT 3

extern const Cursor cursors[CURSORS_COUNT];

void cursors_create(Display *display);
void cursors_destroy(Display *display);

#endif // _CONFIG_CURSORS_H
