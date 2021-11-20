#ifndef _DWM_HANDLERS_C
#define _DWM_HANDLERS_C

void on_button_press(XEvent *e)
{
	unsigned int i, click;
	Client *c;
	Monitor *m;
	XButtonPressedEvent *ev = &e->xbutton;

	click = ClkRootWin;
	/* focus monitor if necessary */
	if ((m = wintomon(ev->window)) && m != selmon
		&& (settings_get_focus_on_wheel() || (ev->button != Button4 && ev->button != Button5))) {
		unfocus(selmon->sel, 1);
		selmon = m;
		focus(NULL);
	}
	if ((c = wintoclient(ev->window))) {
		if (settings_get_focus_on_wheel() || (ev->button != Button4 && ev->button != Button5))
			focus(c);
		XAllowEvents(dpy, ReplayPointer, CurrentTime);
		click = ClkClientWin;
	}
	for (i = 0; i < LENGTH(buttons); i++)
		if (
			click == buttons[i].click
			&&
			buttons[i].func
			&&
			buttons[i].button == ev->button
			&&
			CLEANMASK(buttons[i].mask) == CLEANMASK(ev->state)
		)
			buttons[i].func(&buttons[i].arg);
}

void on_client_message(XEvent *e)
{
	XClientMessageEvent *cme = &e->xclient;
	Client *c = wintoclient(cme->window);

	if (!c) return;

	if (cme->message_type == atoms->netatom[NetWMState]) {
		if (
			cme->data.l[1] == atoms->netatom[NetWMFullscreen]
			||
			cme->data.l[2] == atoms->netatom[NetWMFullscreen]
		) {
			setfullscreen(
				c,
				(
					cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
					||
					cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */
				)
			);
		}
	} else if (cme->message_type == atoms->netatom[NetActiveWindow]) {
		if (c != selmon->sel && !c->state.isurgent) {
			seturgent(c, 1);
		}
	}
}

void on_configure_request(XEvent *e)
{
	Client *c;
	Monitor *m;
	XConfigureRequestEvent *ev = &e->xconfigurerequest;
	XWindowChanges wc;

	if ((c = wintoclient(ev->window))) {
		if (ev->value_mask & CWBorderWidth) {
			c->geometry.bw = ev->border_width;
		} else if (c->state.isfloating || !selmon->lt[selmon->sellt]->arrange) {
			m = c->mon;
			if (ev->value_mask & CWX) {
				c->geometry.basic.x = m->screen_geometry.x + ev->x;
			}
			if (ev->value_mask & CWY) {
				c->geometry.basic.y = m->screen_geometry.y + ev->y;
			}
			if (ev->value_mask & CWWidth) {
				c->geometry.basic.w = ev->width;
			}
			if (ev->value_mask & CWHeight) {
				c->geometry.basic.h = ev->height;
			}
			if (
				(c->geometry.basic.x + c->geometry.basic.w)
				>
				m->screen_geometry.x + m->screen_geometry.w && c->state.isfloating
			) {
				/* center in x direction */
				c->geometry.basic.x =
					m->screen_geometry.x
					+
					(m->screen_geometry.w / 2 - WIDTH(c) / 2);
			}
			if (
				(c->geometry.basic.y + c->geometry.basic.h)
				>
				m->screen_geometry.y + m->screen_geometry.h && c->state.isfloating
			) {
				/* center in y direction */
				c->geometry.basic.y =
					m->screen_geometry.y
					+
					(m->screen_geometry.h / 2 - HEIGHT(c) / 2);
			}
			if ((ev->value_mask & (CWX|CWY)) && !(ev->value_mask & (CWWidth|CWHeight))) {
				configure(c);
			}
			if (ISVISIBLE(c)) {
				XMoveResizeWindow(
					dpy,
					c->win,
					c->geometry.basic.x,
					c->geometry.basic.y,
					c->geometry.basic.w,
					c->geometry.basic.h
				);
			}
		} else
			configure(c);
	} else {
		wc.x = ev->x;
		wc.y = ev->y;
		wc.width = ev->width;
		wc.height = ev->height;
		wc.border_width = ev->border_width;
		wc.sibling = ev->above;
		wc.stack_mode = ev->detail;
		XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
	}
	XSync(dpy, False);
}

void on_configure_notify(XEvent *e)
{
	XConfigureEvent *ev = &e->xconfigure;
	int dirty;

	/* TODO: updategeom handling sucks, needs to be simplified */
	if (ev->window == root) {
		dirty = (sw != ev->width || sh != ev->height);
		sw = ev->width;
		sh = ev->height;
		if (updategeom() || dirty) {
			focus(NULL);
			arrange(NULL);
		}
	}
}

void on_destroy_notify(XEvent *e)
{
	XDestroyWindowEvent *const ev = &e->xdestroywindow;

	Client *c;

	if ((c = wintoclient(ev->window))) {
		unmanage(c, 1);
	}
}

/* there are some broken focus acquiring clients needing extra handling */
void on_focus_in(XEvent *e)
{
	XFocusChangeEvent *ev = &e->xfocus;

	if (selmon->sel && ev->window != selmon->sel->win)
		setfocus(selmon->sel);
}

void on_key_press(XEvent *e)
{
	unsigned int i;
	KeySym keysym;
	XKeyEvent *ev;

	ev = &e->xkey;
	keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
	for (i = 0; i < LENGTH(keys); i++)
		if (keysym == keys[i].keysym
		&& CLEANMASK(keys[i].mod) == CLEANMASK(ev->state)
		&& keys[i].func)
			keys[i].func(&(keys[i].arg));
}

void on_mapping_notify(XEvent *e)
{
	XMappingEvent *ev = &e->xmapping;

	XRefreshKeyboardMapping(ev);
	if (ev->request == MappingKeyboard)
		grabkeys();
}

void on_map_request(XEvent *e)
{
	static XWindowAttributes wa;
	XMapRequestEvent *ev = &e->xmaprequest;

	if (!XGetWindowAttributes(dpy, ev->window, &wa))
		return;
	if (wa.override_redirect)
		return;
	if (!wintoclient(ev->window))
		manage(ev->window, &wa);
}

void on_property_notify(XEvent *e)
{
	Client *c;
	Window trans;
	XPropertyEvent *ev = &e->xproperty;

	if (ev->state == PropertyDelete)
		return; /* ignore */
	else if ((c = wintoclient(ev->window))) {
		switch(ev->atom) {
		default: break;
		case XA_WM_TRANSIENT_FOR:
			if (!c->state.isfloating && (XGetTransientForHint(dpy, c->win, &trans)) &&
				(c->state.isfloating = (wintoclient(trans)) != NULL))
				arrange(c->mon);
			break;
		case XA_WM_NORMAL_HINTS:
			updatesizehints(c);
			break;
		case XA_WM_HINTS:
			updatewmhints(c);
			break;
		}
		if (ev->atom == XA_WM_NAME || ev->atom == atoms->netatom[NetWMName]) {
			updatetitle(c);
		}
		if (ev->atom == atoms->netatom[NetWMWindowType])
			updatewindowtype(c);
	}
}

void on_unmap_notify(XEvent *e)
{
	Client *c;
	XUnmapEvent *ev = &e->xunmap;

	if ((c = wintoclient(ev->window))) {
		if (ev->send_event)
			setclientstate(c, WithdrawnState);
		else
			unmanage(c, 0);
	}
}

#endif // _DWM_HANDLERS_C
