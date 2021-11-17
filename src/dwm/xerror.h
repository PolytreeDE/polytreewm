#ifndef _DWM_XERROR_H
#define _DWM_XERROR_H

static int xerror(Display *dpy, XErrorEvent *ee);
static int xerrordummy(Display *dpy, XErrorEvent *ee);
static int xerrorstart(Display *dpy, XErrorEvent *ee);

#endif // _DWM_XERROR_H
