#ifndef _ATOMS_H
#define _ATOMS_H

#include <X11/Xutil.h>

/* EWMH atoms */
enum {
	NetSupported, NetWMName, NetWMState, NetWMCheck, NetSystemTray,
	NetSystemTrayOP, NetSystemTrayOrientation, NetSystemTrayOrientationHorz,
	NetWMFullscreen, NetActiveWindow, NetWMWindowType, NetWMWindowTypeDialog,
	NetClientList, NetLast,
};

/* Xembed atoms */
enum { Manager, Xembed, XembedInfo, XLast };

/* default atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast };

extern Atom wmatom[WMLast], netatom[NetLast], xatom[XLast];

void atoms_setup(Display *dpy);

#endif // _ATOMS_H
