#ifndef _DWM_BAR_C
#define _DWM_BAR_C

void
togglebar(const Arg *arg)
{
	unit_toggle_show_bar(selmon->unit);

	updatebars();
}

void
updatebar(Monitor *m)
{
	m->show_bar = unit_get_show_bar(m->unit);

	updatebarpos(m);
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
