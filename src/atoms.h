#ifndef _ATOMS_H
#define _ATOMS_H

#include <stddef.h>
#include <X11/Xutil.h>

#define ATOMS_DELETE(atoms) { \
	atoms_delete(atoms);      \
	atoms = NULL;             \
}

/* EWMH atoms */
enum {
	NetSupported, NetWMName, NetWMState, NetWMCheck,
	NetWMFullscreen, NetActiveWindow, NetWMWindowType, NetWMWindowTypeDialog,
	NetClientList, NetLast,
};

/* default atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast };

typedef struct Atoms {
	Atom wmatom[WMLast], netatom[NetLast], utf8string;
} *Atoms;

Atoms atoms_create(Display *dpy);
void atoms_delete(Atoms atoms);

#endif // _ATOMS_H
