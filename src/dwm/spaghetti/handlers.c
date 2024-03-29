#ifndef _DWM_HANDLERS_C
#define _DWM_HANDLERS_C

void on_button_press(XEvent *e)
{
	unsigned int click;
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
		if (
			settings_get_focus_on_wheel()
			||
			(ev->button != Button4 && ev->button != Button5)
		) {
			focus(c);
		}
		XAllowEvents(xbase->x_display, ReplayPointer, CurrentTime);
		click = ClkClientWin;
	}
	for (size_t i = 0; i < buttons_count(); ++i)
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

	if (cme->message_type == xbase->atoms->netatom[NetWMState]) {
		if (
			cme->data.l[1] == xbase->atoms->netatom[NetWMFullscreen]
			||
			cme->data.l[2] == xbase->atoms->netatom[NetWMFullscreen]
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
	} else if (cme->message_type == xbase->atoms->netatom[NetActiveWindow]) {
		if (c != selmon->sel && !c->state.is_urgent) {
			seturgent(c, true);
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
			c->state.geom.border_width = ev->border_width;
		} else if (c->state.is_floating || !selmon->lt[selmon->sellt]->arrange) {
			m = c->mon;
			if (ev->value_mask & CWX) {
				c->state.geom.basic.position.x =
					m->screen_geom.position.x + ev->x;
			}
			if (ev->value_mask & CWY) {
				c->state.geom.basic.position.y =
					m->screen_geom.position.y + ev->y;
			}
			if (ev->value_mask & CWWidth) {
				c->state.geom.basic.sizes.w = ev->width;
			}
			if (ev->value_mask & CWHeight) {
				c->state.geom.basic.sizes.h = ev->height;
			}
			if (
				(
					c->state.geom.basic.position.x
					+
					c->state.geom.basic.sizes.w
				)
				>
				m->screen_geom.position.x + m->screen_geom.sizes.w
				&&
				c->state.is_floating
			) {
				/* center in x direction */
				c->state.geom.basic.position.x =
					m->screen_geom.position.x
					+
					(
						m->screen_geom.sizes.w / 2
						-
						win_geom_total_width(&c->state.geom) / 2
					);
			}
			if (
				(
					c->state.geom.basic.position.y
					+
					c->state.geom.basic.sizes.h
				)
				>
				m->screen_geom.position.y + m->screen_geom.sizes.h
				&&
				c->state.is_floating
			) {
				/* center in y direction */
				c->state.geom.basic.position.y =
					m->screen_geom.position.y
					+
					(
						m->screen_geom.sizes.h / 2
						-
						win_geom_total_height(&c->state.geom) / 2
					);
			}
			if ((ev->value_mask & (CWX|CWY)) && !(ev->value_mask & (CWWidth|CWHeight))) {
				configure(c);
			}
			if (ISVISIBLE(c)) {
				XMoveResizeWindow(
					xbase->x_display,
					c->x_window,
					c->state.geom.basic.position.x,
					c->state.geom.basic.position.y,
					c->state.geom.basic.sizes.w,
					c->state.geom.basic.sizes.h
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
		XConfigureWindow(xbase->x_display, ev->window, ev->value_mask, &wc);
	}
	XSync(xbase->x_display, False);
}

void on_configure_notify(XEvent *e)
{
	XConfigureEvent *ev = &e->xconfigure;
	int dirty;

	/* TODO: updategeom handling sucks, needs to be simplified */
	if (ev->window == xbase->x_root) {
		dirty =
			xbase->screen_sizes.w != ev->width ||
			xbase->screen_sizes.h != ev->height;

		xbase->screen_sizes.w = ev->width;
		xbase->screen_sizes.h = ev->height;

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

	if (selmon->sel && ev->window != selmon->sel->x_window)
		setfocus(selmon->sel);
}

void on_key_press(XEvent *e)
{
	KeySym keysym;
	XKeyEvent *ev;

	ev = &e->xkey;
	keysym = XKeycodeToKeysym(xbase->x_display, (KeyCode)ev->keycode, 0);
	for (size_t i = 0; i < keys_count(); ++i) {
		if (
			keysym == keys[i].keysym
			&&
			CLEANMASK(keys[i].mod) == CLEANMASK(ev->state)
			&&
			keys[i].func
		) {
			keys[i].func(&(keys[i].arg));
		}
	}
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

	if (!XGetWindowAttributes(xbase->x_display, ev->window, &wa)) return;
	if (wa.override_redirect) return;

	if (winpolybar(ev->window)) {
		managepolybar(ev->window, &wa);
	} else if (!wintoclient(ev->window)) {
		manage(ev->window, &wa);
	}
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
			if (
				!c->state.is_floating
				&&
				(XGetTransientForHint(xbase->x_display, c->x_window, &trans))
				&&
				(c->state.is_floating = (wintoclient(trans)) != NULL)
			) {
				arrange(c->mon);
			}
			break;
		case XA_WM_NORMAL_HINTS:
			updatesizehints(c);
			break;
		case XA_WM_HINTS:
			updatewmhints(c);
			break;
		}
		if (ev->atom == XA_WM_NAME || ev->atom == xbase->atoms->netatom[NetWMName]) {
			updatetitle(c);
		}
		if (ev->atom == xbase->atoms->netatom[NetWMWindowType])
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
