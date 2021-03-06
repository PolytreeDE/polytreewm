#ifndef _DWM_XERROR_C
#define _DWM_XERROR_C

/* There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's). Other types of errors call Xlibs
 * default error handler, which may call exit. */
int xerror(Display *const dpy, XErrorEvent *const ee)
{
	if (ee->error_code == BadWindow
		|| (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
		|| (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
		|| (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
		|| (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
		|| (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
		|| (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
		|| (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
		|| (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
	{
		return 0;
	}

	fatal_nodie(
		"request code=%d, error code=%d\n",
		ee->request_code,
		ee->error_code
	);

	return xbase->x_error(dpy, ee); /* may call exit */
}

int xerrordummy(
	__attribute__((unused)) Display *const dpy,
	__attribute__((unused)) XErrorEvent *const ee
) {
	return 0;
}

#endif // _DWM_XERROR_C
