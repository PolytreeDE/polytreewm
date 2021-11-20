#ifndef _DWM_LAYOUTS_C
#define _DWM_LAYOUTS_C

void centeredmaster(Monitor *m)
{
	unsigned int n = 0;
	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next), ++n);
	if (n == 0) return;

	const float master_area_factor = unit_get_master_area_factor(m->unit);

	unsigned int mx = 0;
	unsigned int mw = m->window_area_geometry.w;
	unsigned int tw = mw;
	if (n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		mw = m->nmaster ? m->window_area_geometry.w * master_area_factor : 0;
		tw = m->window_area_geometry.w - mw;

		if (n - m->nmaster > 1) {
			/* only one client */
			mx = (m->window_area_geometry.w - mw) / 2;
			tw = (m->window_area_geometry.w - mw) / 2;
		}
	}

	const bool is_fullscreen = m->sel == NULL ? false : m->sel->isfullscreen;

	const int gap_size = helpers_gap_size(n, is_fullscreen, is_fullscreen);
	const int border_width = helpers_border_width(n, is_fullscreen, is_fullscreen);

	const int top_left_half_gap = gap_size / 2;
	const int bottom_right_half_gap = gap_size - top_left_half_gap;

	Client *c = nexttiled(m->clients);
	unsigned int oty = 0, ety = 0, my = 0;

	for (unsigned int i = 0; c; c = nexttiled(c->next), ++i) {
		if (i < m->nmaster) {
			// nmaster clients are stacked vertically,
			// in the center of the screen
			const unsigned int h = (m->window_area_geometry.h - my) / (MIN(n, m->nmaster) - i);

			const int left_gap   = (n <= m->nmaster + 1) ? gap_size : top_left_half_gap;
			const int top_gap    = i == 0 ? gap_size : top_left_half_gap;
			const int right_gap  = n <= m->nmaster ? gap_size : bottom_right_half_gap;
			const int bottom_gap = (i == m->nmaster - 1 || i == n - 1) ? gap_size : bottom_right_half_gap;

			resize(
				c,
				m->window_area_geometry.x + mx + left_gap,
				m->window_area_geometry.y + my + top_gap,
				mw - 2 * border_width - left_gap - right_gap,
				h - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			my += HEIGHT(c) + top_gap + bottom_gap;
		} else {
			// stack clients are stacked vertically
			if ((i - m->nmaster) % 2) {
				const unsigned int h = (m->window_area_geometry.h - ety) / ((1 + n - i) / 2);

				const int left_gap   = gap_size;
				const int top_gap    = (i == m->nmaster + 1) ? gap_size : top_left_half_gap;
				const int right_gap  = bottom_right_half_gap;
				const int bottom_gap = (i == n - 1 || i == n - 2) ? gap_size : bottom_right_half_gap;

				resize(
					c,
					m->window_area_geometry.x + left_gap,
					m->window_area_geometry.y + ety + top_gap,
					tw - 2 * border_width - left_gap - right_gap,
					h - 2 * border_width - top_gap - bottom_gap,
					border_width,
					0
				);

				ety += HEIGHT(c) + top_gap + bottom_gap;
			} else {
				const unsigned int h = (m->window_area_geometry.h - oty) / ((1 + n - i) / 2);

				const int left_gap   = (m->nmaster == 0 && n == 1) ? gap_size : top_left_half_gap;
				const int top_gap    = i == m->nmaster ? gap_size : top_left_half_gap;
				const int right_gap  = gap_size;
				const int bottom_gap = (i == n - 1 || i == n - 2) ? gap_size : bottom_right_half_gap;

				resize(
					c,
					m->window_area_geometry.x + mx + mw + left_gap,
					m->window_area_geometry.y + oty + top_gap,
					tw - 2 * border_width - left_gap - right_gap,
					h - 2 * border_width - top_gap - bottom_gap,
					border_width,
					0
				);

				oty += HEIGHT(c) + top_gap + bottom_gap;
			}
		}
	}
}

void floating(Monitor *m)
{
	const int border_width = settings_get_border_width();

	for (Client *c = m->clients; c; c = c->next) {
		if (ISVISIBLE(c) && c->geometry.bw == 0) {
			resize(
				c,
				c->geometry.basic.x,
				c->geometry.basic.y,
				c->geometry.basic.w - 2 * border_width,
				c->geometry.basic.h - 2 * border_width,
				border_width,
				0
			);
		}
	}
}

void horizontile(Monitor *m)
{
	unsigned int n = 0;
	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next), ++n);
	if (n == 0) return;

	const float master_area_factor = unit_get_master_area_factor(m->unit);

	const bool is_fullscreen = m->sel == NULL ? false : m->sel->isfullscreen;

	const int gap_size = helpers_gap_size(n, is_fullscreen, is_fullscreen);
	const int border_width = helpers_border_width(n, is_fullscreen, is_fullscreen);

	const int top_left_half_gap = gap_size / 2;
	const int bottom_right_half_gap = gap_size - top_left_half_gap;

	const unsigned int mh = n > m->nmaster ? (m->nmaster ? m->window_area_geometry.h * master_area_factor : 0) : m->window_area_geometry.h;

	Client *c = nexttiled(m->clients);

	for (unsigned int i = 0, mx = 0, tx = 0; c; c = nexttiled(c->next), ++i) {
		if (i < m->nmaster) {
			const unsigned int w = (m->window_area_geometry.w - mx) / (MIN(n, m->nmaster) - i);

			const unsigned int left_gap   = i == 0 ? gap_size : top_left_half_gap;
			const unsigned int top_gap    = gap_size;
			const unsigned int right_gap  = (i == m->nmaster - 1 || i == n - 1) ? gap_size : bottom_right_half_gap;
			const unsigned int bottom_gap = n <= m->nmaster ? gap_size : bottom_right_half_gap;

			resize(
				c,
				m->window_area_geometry.x + mx + left_gap,
				m->window_area_geometry.y + top_gap,
				w - 2 * border_width - left_gap - right_gap,
				mh - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			// FIXME: maybe need + left_gap + right_gap
			if (mx + WIDTH(c) < m->window_area_geometry.w) {
				mx += WIDTH(c) + left_gap + right_gap;
			}
		} else {
			const unsigned int w = (m->window_area_geometry.w - tx) / (n - i);

			const unsigned int left_gap   = i == m->nmaster ? gap_size : top_left_half_gap;
			const unsigned int top_gap    = m->nmaster == 0 ? gap_size : top_left_half_gap;
			const unsigned int right_gap  = (i == n - 1) ? gap_size : bottom_right_half_gap;
			const unsigned int bottom_gap = gap_size;

			resize(
				c,
				m->window_area_geometry.x + tx + left_gap,
				m->window_area_geometry.y + mh + top_gap,
				w - 2 * border_width - left_gap - right_gap,
				m->window_area_geometry.h - mh - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			// FIXME: maybe need + left_gap + right_gap
			if (tx + WIDTH(c) < m->window_area_geometry.w) {
				tx += WIDTH(c) + left_gap + right_gap;
			}
		}
	}
}

void monocle(Monitor *m)
{
	bool any_is_fullscreen = false;
	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		any_is_fullscreen = any_is_fullscreen || c->isfullscreen;
	}

	const bool is_fullscreen = m->sel == NULL ? false : m->sel->isfullscreen;

	const int gap_size = helpers_gap_size(1, is_fullscreen, any_is_fullscreen);
	const int border_width = helpers_border_width(1, is_fullscreen, any_is_fullscreen);

	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		resize(
			c,
			m->window_area_geometry.x + gap_size,
			m->window_area_geometry.y + gap_size,
			m->window_area_geometry.w - 2 * border_width - 2 * gap_size,
			m->window_area_geometry.h - 2 * border_width - 2 * gap_size,
			border_width,
			0
		);
	}
}

void tile(Monitor *m)
{
	unsigned int n = 0;
	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next), ++n);
	if (n == 0) return;

	const float master_area_factor = unit_get_master_area_factor(m->unit);

	const bool is_fullscreen = m->sel == NULL ? false : m->sel->isfullscreen;

	const int gap_size = helpers_gap_size(n, is_fullscreen, is_fullscreen);
	const int border_width = helpers_border_width(n, is_fullscreen, is_fullscreen);

	const int top_left_half_gap = gap_size / 2;
	const int bottom_right_half_gap = gap_size - top_left_half_gap;

	const unsigned int mw = n > m->nmaster ? (m->nmaster ? m->window_area_geometry.w * master_area_factor : 0) : m->window_area_geometry.w;

	Client *c = nexttiled(m->clients);

	for (unsigned int i = 0, my = 0, ty = 0; c; c = nexttiled(c->next), ++i) {
		if (i < m->nmaster) {
			const unsigned int h = (m->window_area_geometry.h - my) / (MIN(n, m->nmaster) - i);

			const unsigned int left_gap   = gap_size;
			const unsigned int top_gap    = i == 0 ? gap_size : top_left_half_gap;
			const unsigned int right_gap  = n <= m->nmaster ? gap_size : bottom_right_half_gap;
			const unsigned int bottom_gap = (i == m->nmaster - 1 || i == n - 1) ? gap_size : bottom_right_half_gap;

			resize(
				c,
				m->window_area_geometry.x + left_gap,
				m->window_area_geometry.y + my + top_gap,
				mw - 2 * border_width - left_gap - right_gap,
				h - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			// FIXME: maybe need + top_gap + bottom_gap
			if (my + HEIGHT(c) < m->window_area_geometry.h) {
				my += HEIGHT(c) + top_gap + bottom_gap;
			}
		} else {
			const unsigned int h = (m->window_area_geometry.h - ty) / (n - i);

			const unsigned int left_gap   = m->nmaster == 0 ? gap_size : top_left_half_gap;
			const unsigned int top_gap    = i == m->nmaster ? gap_size : top_left_half_gap;
			const unsigned int right_gap  = gap_size;
			const unsigned int bottom_gap = (i == n - 1) ? gap_size : bottom_right_half_gap;

			resize(
				c,
				m->window_area_geometry.x + mw + left_gap,
				m->window_area_geometry.y + ty + top_gap,
				m->window_area_geometry.w - mw - 2 * border_width - left_gap - right_gap,
				h - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			// FIXME: maybe need + top_gap + bottom_gap
			if (ty + HEIGHT(c) < m->window_area_geometry.h) {
				ty += HEIGHT(c) + top_gap + bottom_gap;
			}
		}
	}
}

#endif // _DWM_LAYOUTS_C
