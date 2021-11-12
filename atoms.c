#include "atoms.h"

#include <stdlib.h>

Atom wmatom[WMLast], netatom[NetLast], xatom[XLast];

AtomValues atoms_create(Display *const dpy)
{
	AtomValues atom_values = malloc(sizeof(struct AtomValues));

	atom_values->wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", False);
	atom_values->wmatom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	atom_values->wmatom[WMState] = XInternAtom(dpy, "WM_STATE", False);
	atom_values->wmatom[WMTakeFocus] = XInternAtom(dpy, "WM_TAKE_FOCUS", False);

	atom_values->netatom[NetActiveWindow] = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
	atom_values->netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", False);
	atom_values->netatom[NetSystemTray] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_S0", False);
	atom_values->netatom[NetSystemTrayOP] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_OPCODE", False);
	atom_values->netatom[NetSystemTrayOrientation] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_ORIENTATION", False);
	atom_values->netatom[NetSystemTrayOrientationHorz] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_ORIENTATION_HORZ", False);
	atom_values->netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", False);
	atom_values->netatom[NetWMState] = XInternAtom(dpy, "_NET_WM_STATE", False);
	atom_values->netatom[NetWMCheck] = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
	atom_values->netatom[NetWMFullscreen] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	atom_values->netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
	atom_values->netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
	atom_values->netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", False);
	atom_values->netatom[NetDateTime] = XInternAtom(dpy, "_NET_DATE_TIME", False);

	atom_values->xatom[Manager] = XInternAtom(dpy, "MANAGER", False);
	atom_values->xatom[Xembed] = XInternAtom(dpy, "_XEMBED", False);
	atom_values->xatom[XembedInfo] = XInternAtom(dpy, "_XEMBED_INFO", False);

	return atom_values;
}

void atoms_destroy(AtomValues atom_values)
{
	free(atom_values);
}
