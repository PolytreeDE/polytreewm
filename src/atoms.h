#ifndef _ATOMS_H
#define _ATOMS_H

#include <stddef.h>
#include <X11/Xutil.h>

#define ATOMS_DELETE(atoms) { \
	atoms_delete(atoms);      \
	atoms = NULL;             \
}

/* EWMH atoms */
#define NetSupported          0
#define NetWMName             1
#define NetWMState            2
#define NetWMCheck            3
#define NetWMFullscreen       4
#define NetActiveWindow       5
#define NetWMWindowType       6
#define NetWMWindowTypeDialog 7
#define NetClientList         8
#define NetLast               9

/* default atoms */
#define WMProtocols 0
#define WMDelete    1
#define WMState     2
#define WMTakeFocus 3
#define WMLast      4

typedef struct Atoms {
	Atom wmatom[WMLast], netatom[NetLast], utf8string;
} *Atoms;

Atoms atoms_create(Display *dpy);
void atoms_delete(Atoms atoms);

#endif // _ATOMS_H
