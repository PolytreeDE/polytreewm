#include "atoms.h"

#include <stdlib.h>

Atoms atoms_create(Display *const dpy)
{
	Atoms atoms = malloc(sizeof(struct Atoms));

	atoms->wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", False);
	atoms->wmatom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	atoms->wmatom[WMState] = XInternAtom(dpy, "WM_STATE", False);
	atoms->wmatom[WMTakeFocus] = XInternAtom(dpy, "WM_TAKE_FOCUS", False);

	atoms->netatom[NetActiveWindow] = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
	atoms->netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", False);
	atoms->netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", False);
	atoms->netatom[NetWMState] = XInternAtom(dpy, "_NET_WM_STATE", False);
	atoms->netatom[NetWMCheck] = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
	atoms->netatom[NetWMFullscreen] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	atoms->netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
	atoms->netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
	atoms->netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", False);

	atoms->utf8string = XInternAtom(dpy, "UTF8_STRING", False);

	return atoms;
}

void atoms_delete(Atoms atoms)
{
	// TODO: maybe we should assert
	if (atoms == NULL) return;

	free(atoms);
}
