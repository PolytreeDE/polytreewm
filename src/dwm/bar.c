#ifndef _DWM_BAR_C
#define _DWM_BAR_C

void
createbars(void)
{
	XClassHint ch = {"polytreewm", "polytreewm"};
	XSetWindowAttributes wa = {
		.override_redirect = True,
		.background_pixmap = ParentRelative,
		.event_mask = ButtonPressMask|ExposureMask
	};

	for (Monitor *m = mons; m; m = m->next) {
		if (m->bar->barwin) continue;

		m->bar->barwin = XCreateWindow(
			dpy,
			root,
			m->wx,
			m->bar->by,
			m->ww,
			m->bar->bh,
			0,
			DefaultDepth(dpy, screen),
			CopyFromParent,
			DefaultVisual(dpy, screen),
			CWOverrideRedirect | CWBackPixmap | CWEventMask,
			&wa
		);

		XDefineCursor(dpy, m->bar->barwin, cursor[CurNormal]->cursor);
		XMapRaised(dpy, m->bar->barwin);
		XSetClassHint(dpy, m->bar->barwin, &ch);
	}
}

void
drawbar(Monitor *m)
{
	int x, w, tw = 0;
	unsigned int i, occ = 0, urg = 0;
	Client *c;

	for (c = m->clients; c; c = c->next) {
		occ |= c->tags == 255 ? 0 : c->tags;
		if (c->isurgent)
			urg |= c->tags;
	}
	x = 0;
	for (i = 0; i < TAGS_COUNT; i++) {
		/* do not draw vacant tags */
		if (!(occ & 1 << i || m->tagset[m->seltags] & 1 << i || tags_get(i)->has_custom_name))
		continue;

		w = TEXTW(tags_get(i)->name.cstr);
		drw_setscheme(drw, scheme[m->tagset[m->seltags] & 1 << i ? SchemeSel : SchemeNorm]);
		drw_text(drw, x, 0, w, m->bar->bh, lrpad / 2, tags_get(i)->name.cstr, urg & 1 << i);
		x += w;
	}
	w = TEXTW(m->ltsymbol);
	drw_setscheme(drw, scheme[SchemeNorm]);
	x = drw_text(drw, x, 0, w, m->bar->bh, lrpad / 2, m->ltsymbol, 0);

	if ((w = m->ww - tw - x) > m->bar->bh) {
		drw_setscheme(drw, scheme[SchemeNorm]);
		drw_rect(drw, x, 0, w, m->bar->bh, 1, 1);
	}

	drw_map(drw, m->bar->barwin, 0, 0, m->ww, m->bar->bh);
}

void
drawbars(void)
{
	for (Monitor *m = mons; m; m = m->next) {
		drawbar(m);
	}
}

void
togglebar(const Arg *arg)
{
	unit_toggle_show_bar(selmon->pertag->units[selmon->pertag->curtag]);

	updatebars();
}

void
updatebar(Monitor *m)
{
	m->show_bar = unit_get_show_bar(m->pertag->units[m->pertag->curtag]);

	updatebarpos(m);
	XMoveResizeWindow(dpy, selmon->bar->barwin, selmon->wx, selmon->bar->by, selmon->ww, m->bar->bh);
	arrange(m);
}

void
updatebars()
{
	for (Monitor *m = mons; m; m = m->next) {
		updatebar(m);
	}
}

void
updatebarpos(Monitor *m)
{
	m->wy = m->my;
	m->wh = m->mh;
	if (m->show_bar) {
		m->wh -= m->bar->bh;
		m->bar->by = m->bar->topbar ? m->wy : m->wy + m->wh;
		m->wy = m->bar->topbar ? m->wy + m->bar->bh : m->wy;
	} else
		m->bar->by = -m->bar->bh;
}

#endif // _DWM_BAR_C
