#include "atoms.h"

#include <stdlib.h>

static const char *const default_atoms[WMLast] = {
	[WMProtocols] = "WM_PROTOCOLS",
	[WMDelete]    = "WM_DELETE_WINDOW",
	[WMState]     = "WM_STATE",
	[WMTakeFocus] = "WM_TAKE_FOCUS",
};

static const char *const ewmh_atoms[NetLast] = {
	[NetActiveWindow]       = "_NET_ACTIVE_WINDOW",
	[NetSupported]          = "_NET_SUPPORTED",
	[NetWMName]             = "_NET_WM_NAME",
	[NetWMState]            = "_NET_WM_STATE",
	[NetWMCheck]            = "_NET_SUPPORTING_WM_CHECK",
	[NetWMFullscreen]       = "_NET_WM_STATE_FULLSCREEN",
	[NetWMWindowType]       = "_NET_WM_WINDOW_TYPE",
	[NetWMWindowTypeDialog] = "_NET_WM_WINDOW_TYPE_DIALOG",
	[NetClientList]         = "_NET_CLIENT_LIST",
};

Atoms atoms_create(Display *const dpy)
{
	Atoms atoms = malloc(sizeof(struct Atoms));

	for (int index = 0; index < WMLast; ++index) {
		atoms->wmatom[index] = XInternAtom(dpy, default_atoms[index], False);
	}

	for (int index = 0; index < NetLast; ++index) {
		atoms->netatom[index] = XInternAtom(dpy, ewmh_atoms[index], False);
	}

	atoms->utf8string = XInternAtom(dpy, "UTF8_STRING", False);

	return atoms;
}

void atoms_delete(Atoms atoms)
{
	// TODO: maybe we should assert
	if (atoms == NULL) return;

	free(atoms);
}
