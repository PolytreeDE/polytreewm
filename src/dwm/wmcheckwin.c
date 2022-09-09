static Window wmcheckwin = 0;

void wmcheckwin_create()
{
	wmcheckwin = XCreateSimpleWindow(
		xbase->x_display,
		xbase->x_root,
		0,
		0,
		1,
		1,
		0,
		0,
		0
	);

	XChangeProperty(
		xbase->x_display,
		wmcheckwin,
		xbase->atoms->netatom[NetWMCheck],
		XA_WINDOW,
		32,
		PropModeReplace,
		(unsigned char*)&wmcheckwin,
		1
	);
	XChangeProperty(
		xbase->x_display,
		wmcheckwin,
		xbase->atoms->netatom[NetWMName],
		xbase->atoms->utf8string,
		8,
		PropModeReplace,
		(unsigned char*)
		xbase->program_title,
		strlen(xbase->program_title)
	);
	XChangeProperty(
		xbase->x_display,
		xbase->x_root,
		xbase->atoms->netatom[NetWMCheck],
		XA_WINDOW,
		32,
		PropModeReplace,
		(unsigned char*)&wmcheckwin,
		1
	);
}

void wmcheckwin_destroy()
{
	XDestroyWindow(xbase->x_display, wmcheckwin);
}
