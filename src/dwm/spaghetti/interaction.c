void configborder(const Arg *const arg)
{
	if (arg == NULL) return;
	const int old_border_width = settings_get_border_width();
	const int new_border_width = old_border_width + (arg->i >= 0 ? +1 : -1);
	settings_set_border_width(new_border_width);
	arrange(selmon);
}

void configgap(const Arg *const arg)
{
	if (arg == NULL) return;
	const int old_gap_size = settings_get_gap_size();
	const int new_gap_size = old_gap_size + (arg->i >= 0 ? +2 : -2);
	settings_set_gap_size(new_gap_size);
	arrange(selmon);
}

void dorestart(__attribute__((unused)) const Arg *const arg)
{
	restart();
}

void focusmon(const Arg *arg)
{
	if (!mons->next) return;

	Monitor *m;
	if ((m = dirtomon(arg->i)) == selmon) return;

	unfocus(selmon->sel, 0);
	selmon = m;
	focus(NULL);
}

void focusstack(const Arg *arg)
{
	if (!selmon->sel) return;

	Client *c = NULL;

	if (arg->i > 0) {
		for (c = selmon->sel->next; c && !ISVISIBLE(c); c = c->next);

		if (!c) {
			for (c = selmon->clients; c && !ISVISIBLE(c); c = c->next);
		}
	} else {
		Client *i = selmon->clients;

		for (; i != selmon->sel; i = i->next) {
			if (ISVISIBLE(i)) {
				c = i;
			}
		}

		if (!c) {
			for (; i; i = i->next) {
				if (ISVISIBLE(i)) {
					c = i;
				}
			}
		}
	}

	if (c) {
		focus(c);
		restack(selmon);

		{
			unsigned int n = 0;
			for (Client *cc = nexttiled(selmon->clients); cc; cc = nexttiled(cc->next), ++n);

			// TODO: Maybe it's an unnecessary optimization
			// and we don't need the condition.
			if (n > 1) {
				// We have to rearrange because borders and gaps may have
				// changed in monocle layout.
				arrange(selmon);
			}
		}
	}
}

void incnmaster(const Arg *arg)
{
	const int max_clients_in_master = settings_get_max_clients_in_master();
	const int new_clients_in_master = MAX(0, selmon->nmaster + arg->i);

	selmon->nmaster =
		max_clients_in_master == 0
		? new_clients_in_master
		: MIN(new_clients_in_master, max_clients_in_master);

	arrange(selmon);
}

void killclient(__attribute__((unused)) const Arg *arg)
{
	if (!selmon->sel)
		return;
	if (!sendevent(selmon->sel, xbase->atoms->wmatom[WMDelete])) {
		XGrabServer(xbase->x_display);
		XSetErrorHandler(xerrordummy);
		XSetCloseDownMode(xbase->x_display, DestroyAll);
		XKillClient(xbase->x_display, selmon->sel->x_window);
		XSync(xbase->x_display, False);
		XSetErrorHandler(xerror);
		XUngrabServer(xbase->x_display);
	}
}

void movemouse(__attribute__((unused)) const Arg *arg)
{
	Client *const c = selmon->sel;
	if (c == NULL) return;

	restack(selmon);

	if (
		XGrabPointer(
			xbase->x_display,
			xbase->x_root,
			False,
			MOUSEMASK,
			GrabModeAsync,
			GrabModeAsync,
			None,
			cursors[CURSOR_MOVE],
			CurrentTime
		) != GrabSuccess
	) {
		return;
	}

	int x, y;
	if (!getrootptr(&x, &y)) return;

	const unsigned int snap_distance = settings_get_snap_distance();
	const int ocx = c->state.geom.basic.position.x;
	const int ocy = c->state.geom.basic.position.y;

	Time lasttime = 0;

	XEvent ev;

	do {
		XMaskEvent(
			xbase->x_display,
			MOUSEMASK | ExposureMask | SubstructureRedirectMask,
			&ev
		);

		switch (ev.type) {
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			if ((ev.xmotion.time - lasttime) <= (1000 / 60)) continue;

			lasttime = ev.xmotion.time;

			int nx = ocx + (ev.xmotion.x - x);
			int ny = ocy + (ev.xmotion.y - y);

			if (
				abs(selmon->window_area_geom.position.x - nx)
				<
				snap_distance
			) {
				nx = selmon->window_area_geom.position.x;
			} else if (
				abs(
					(
						selmon->window_area_geom.position.x
						+
						selmon->window_area_geom.sizes.w
					)
					-
					(nx + win_geom_total_width(&c->state.geom))
				)
				<
				snap_distance
			) {
				nx =
					selmon->window_area_geom.position.x
					+
					selmon->window_area_geom.sizes.w
					-
					win_geom_total_width(&c->state.geom);
			}

			if (
				abs(selmon->window_area_geom.position.y - ny)
				<
				snap_distance
			) {
				ny = selmon->window_area_geom.position.y;
			} else if (
				abs(
					(
						selmon->window_area_geom.position.y
						+
						selmon->window_area_geom.sizes.h
					) - (ny + win_geom_total_height(&c->state.geom))
				)
				<
				snap_distance
			) {
				ny =
					selmon->window_area_geom.position.y
					+
					selmon->window_area_geom.sizes.h
					-
					win_geom_total_height(&c->state.geom);
			}

			if (
				!c->state.is_floating
				&&
				(
					abs(nx - c->state.geom.basic.position.x) > snap_distance
					||
					abs(ny - c->state.geom.basic.position.y) > snap_distance)
			) {
				togglefloating(NULL);
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->state.is_floating) {
				struct WinGeom win_geom = c->state.geom;
				position_init_from_args(&win_geom.basic.position, nx, ny);
				resize(c, win_geom, 1);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XUngrabPointer(xbase->x_display, CurrentTime);

	Monitor *const m = recttomon(
		c->state.geom.basic.position.x,
		c->state.geom.basic.position.y,
		c->state.geom.basic.sizes.w,
		c->state.geom.basic.sizes.h
	);

	if (m != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
}

void movestack(const Arg *arg)
{
	Client *c = NULL, *p = NULL, *pc = NULL, *i = NULL;

	if (arg->i > 0) {
		/* find the client after selmon->sel */
		for (
			c = selmon->sel->next;
			c && (!ISVISIBLE(c) || c->state.is_floating);
			c = c->next
		);

		if(!c) {
			for(
				c = selmon->clients;
				c && (!ISVISIBLE(c) || c->state.is_floating);
				c = c->next
			);
		}
	} else {
		/* find the client before selmon->sel */
		for (i = selmon->clients; i != selmon->sel; i = i->next) {
			if (ISVISIBLE(i) && !i->state.is_floating) {
				c = i;
			}
		}

		if (!c) {
			for (; i; i = i->next) {
				if (ISVISIBLE(i) && !i->state.is_floating) {
					c = i;
				}
			}
		}
	}

	/* find the client before selmon->sel and c */
	for (i = selmon->clients; i && (!p || !pc); i = i->next) {
		if (i->next == selmon->sel) {
			p = i;
		}
		if (i->next == c) {
			pc = i;
		}
	}

	/* swap c and selmon->sel selmon->clients in the selmon->clients list */
	if (c && c != selmon->sel) {
		Client *temp = selmon->sel->next == c ? selmon->sel : selmon->sel->next;
		selmon->sel->next = c->next==selmon->sel?c:c->next;
		c->next = temp;

		if (p && p != c) {
			p->next = c;
		}
		if (pc && pc != selmon->sel) {
			pc->next = selmon->sel;
		}

		if (selmon->sel == selmon->clients) {
			selmon->clients = c;
		} else if (c == selmon->clients) {
			selmon->clients = selmon->sel;
		}

		arrange(selmon);
	}
}

void quit(__attribute__((unused)) const Arg *arg)
{
	running = 0;
}

void resetnmaster(const Arg *arg)
{
	const int max_clients_in_master = settings_get_max_clients_in_master();
	const int new_clients_in_master = arg->i == 0 ? 0 : settings_get_default_clients_in_master();

	selmon->nmaster =
		max_clients_in_master == 0
		? new_clients_in_master
		: MIN(new_clients_in_master, max_clients_in_master);

	arrange(selmon);
}

void resizemouse(__attribute__((unused)) const Arg *arg)
{
	Client *const c = selmon->sel;
	if (c == NULL) return;

	restack(selmon);

	if (
		XGrabPointer(
			xbase->x_display,
			xbase->x_root,
			False,
			MOUSEMASK,
			GrabModeAsync,
			GrabModeAsync,
			None,
			cursors[CURSOR_RESIZE],
			CurrentTime
		) != GrabSuccess
	) {
		return;
	}

	XWarpPointer(
		xbase->x_display,
		None,
		c->x_window,
		0,
		0,
		0,
		0,
		c->state.geom.basic.sizes.w + c->state.geom.border_width - 1,
		c->state.geom.basic.sizes.h + c->state.geom.border_width - 1
	);

	const unsigned int snap_distance = settings_get_snap_distance();
	const int ocx = c->state.geom.basic.position.x;
	const int ocy = c->state.geom.basic.position.y;

	Time lasttime = 0;

	XEvent ev;

	do {
		XMaskEvent(
			xbase->x_display,
			MOUSEMASK | ExposureMask | SubstructureRedirectMask,
			&ev
		);

		switch (ev.type) {
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			if ((ev.xmotion.time - lasttime) <= (1000 / 60)) continue;

			lasttime = ev.xmotion.time;

			const int nw = MAX(
				ev.xmotion.x - ocx - 2 * c->state.geom.border_width + 1,
				1
			);
			const int nh = MAX(
				ev.xmotion.y - ocy - 2 * c->state.geom.border_width + 1,
				1
			);

			if (
				(
					c->mon->window_area_geom.position.x + nw
					>=
					selmon->window_area_geom.position.x
				)
				&&
				(
					c->mon->window_area_geom.position.x + nw
					<=
					(
						selmon->window_area_geom.position.x
						+
						selmon->window_area_geom.sizes.w
					)
				)
				&&
				(
					c->mon->window_area_geom.position.y + nh
					>=
					selmon->window_area_geom.position.y
				)
				&&
				(
					c->mon->window_area_geom.position.y + nh
					<=
					(
						selmon->window_area_geom.position.y
						+
						selmon->window_area_geom.sizes.h
					)
				)
			) {
				if (
					!c->state.is_floating
					&&
					(
						selmon->lt[selmon->sellt]->arrange == NULL
						||
						abs(nw - c->state.geom.basic.sizes.w) > snap_distance
						||
						abs(nh - c->state.geom.basic.sizes.h) > snap_distance
					)
				) {
					togglefloating(NULL);
				}
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->state.is_floating) {
				struct WinGeom win_geom = c->state.geom;
				sizes_init_from_args(&win_geom.basic.sizes, nw, nh);
				resize(c, win_geom, 1);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XWarpPointer(
		xbase->x_display,
		None,
		c->x_window,
		0,
		0,
		0,
		0,
		c->state.geom.basic.sizes.w + c->state.geom.border_width - 1,
		c->state.geom.basic.sizes.h + c->state.geom.border_width - 1
	);

	XUngrabPointer(xbase->x_display, CurrentTime);

	while (XCheckMaskEvent(xbase->x_display, EnterWindowMask, &ev));

	Monitor *const m = recttomon(
		c->state.geom.basic.position.x,
		c->state.geom.basic.position.y,
		c->state.geom.basic.sizes.w,
		c->state.geom.basic.sizes.h
	);

	if (m != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
}

void setlayout(const Arg *arg)
{
	if (!arg || !arg->v || arg->v != selmon->lt[selmon->sellt]) {
		selmon->sellt ^= 1;
	}

	if (arg && arg->v) {
		const Layout *const new_layout = arg->v;
		selmon->lt[selmon->sellt] = new_layout;
	}

	unsigned int visible_clients = 0;
	for (const Client *client = selmon->clients; client; client = client->next) {
		if (ISVISIBLE(client)) ++visible_clients;
	}

	if (selmon->sel) {
		arrange(selmon);
	}
}

void setmfact(const Arg *arg)
{
	if (!arg) return;

	unit_inc_master_area_factor(selmon->unit, arg->f);

	for (Monitor *m = mons; m; m = m->next) {
		arrange(m);
	}
}

void spawn(const Arg *arg)
{
	const char *const command_name = arg->v;

	spawn_command(command_name, spawn_callback, selmon->num);
}

void spawn_callback()
{
	if (xbase->x_display) {
		close(ConnectionNumber(xbase->x_display));
	}
}

void tagmon(const Arg *arg)
{
	if (!selmon->sel || !mons->next)
		return;
	sendmon(selmon->sel, dirtomon(arg->i));
}

void togglefloating(__attribute__((unused)) const Arg *arg)
{
	if (!selmon->sel) return;

	selmon->sel->state.is_floating =
		!selmon->sel->state.is_floating || selmon->sel->state.is_fixed;

	const int border_width = settings_get_border_width();

	if (selmon->sel->state.is_floating) {
		struct WinGeom win_geom = selmon->sel->state.geom;

		win_geom.basic.sizes = sizes_create_from_args(
			selmon->sel->state.geom.basic.sizes.w -
				2 * (border_width - selmon->sel->state.geom.border_width),
			selmon->sel->state.geom.basic.sizes.h -
				2 * (border_width - selmon->sel->state.geom.border_width)
		);

		win_geom.border_width = border_width;

		resize(selmon->sel, win_geom, 0);
	}

	arrange(selmon);
}

void zoom(__attribute__((unused)) const Arg *arg)
{
	Client *c = selmon->sel;

	if (!selmon->lt[selmon->sellt]->arrange
	|| (selmon->sel && selmon->sel->state.is_floating))
		return;
	if (c == nexttiled(selmon->clients))
		if (!c || !(c = nexttiled(c->next)))
			return;
	pop(c);
}
