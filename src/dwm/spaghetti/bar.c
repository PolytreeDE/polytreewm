bool winpolybar(const Window w)
{
	XClassHint ch = { NULL, NULL };
	bool result = true;

	if (XGetClassHint(xbase->x_display, w, &ch)) {
		if (
			ch.res_class
			&&
			strstr(ch.res_class, "Polybar") == NULL
			&&
			strstr(ch.res_class, "polybar") == NULL
		) {
			result = false;
		}

		if (
			ch.res_name
			&&
			strstr(ch.res_name, "Polybar") == NULL
			&&
			strstr(ch.res_name, "polybar") == NULL
		) {
			result = false;
		}
	} else {
		result = false;
	}

	if (ch.res_class) XFree(ch.res_class);
	if (ch.res_name)  XFree(ch.res_name);

	return result;
}
