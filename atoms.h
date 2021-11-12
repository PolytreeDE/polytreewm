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

typedef struct AtomValues {
	Atom wmatom[WMLast], netatom[NetLast], xatom[XLast];
} *AtomValues;

AtomValues atoms_create(Display *dpy);
void atoms_destroy(AtomValues atom_values);

#endif // _ATOMS_H
