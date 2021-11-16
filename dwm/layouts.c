void
centeredmaster(Monitor *m)
{
	unsigned int n = 0;
	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next), ++n);
	if (n == 0) return;

	unsigned int mx = 0;
	unsigned int mw = m->ww;
	unsigned int tw = mw;
	if (n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		mw = m->nmaster ? m->ww * m->mfact : 0;
		tw = m->ww - mw;

		if (n - m->nmaster > 1) {
			/* only one client */
			mx = (m->ww - mw) / 2;
			tw = (m->ww - mw) / 2;
		}
	}

	const SettingsForSingleWindow gap_for_single_window = settings_get_gap_for_single_window();
	const SettingsForSingleWindow border_for_single_window = settings_get_border_for_single_window();

	const int gap_size = (
		n > 1
		||
		gap_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_ALWAYS
		||
		(
			gap_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN
			&&
			!m->sel->isfullscreen
		)
	) ? settings_get_gap_size() : 0;

	const int border_width = (
		n > 1
		||
		border_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_ALWAYS
		||
		(
			border_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN
			&&
			!m->sel->isfullscreen
		)
	) ? settings_get_border_width() : 0;

	const int top_left_half_gap = gap_size / 2;
	const int bottom_right_half_gap = gap_size - top_left_half_gap;

	Client *c = nexttiled(m->clients);
	unsigned int oty = 0, ety = 0, my = 0;

	for (unsigned int i = 0; c; c = nexttiled(c->next), ++i) {
		if (i < m->nmaster) {
			// nmaster clients are stacked vertically,
			// in the center of the screen
			const unsigned int h = (m->wh - my) / (MIN(n, m->nmaster) - i);

			const int left_gap   = (n <= m->nmaster + 1) ? gap_size : top_left_half_gap;
			const int top_gap    = i == 0 ? gap_size : top_left_half_gap;
			const int right_gap  = n <= m->nmaster ? gap_size : bottom_right_half_gap;
			const int bottom_gap = (i == m->nmaster - 1 || i == n - 1) ? gap_size : bottom_right_half_gap;

			resize(
				c,
				m->wx + mx + left_gap,
				m->wy + my + top_gap,
				mw - 2 * border_width - left_gap - right_gap,
				h - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			my += HEIGHT(c) + top_gap + bottom_gap;
		} else {
			// stack clients are stacked vertically
			if ((i - m->nmaster) % 2) {
				const unsigned int h = (m->wh - ety) / ((1 + n - i) / 2);

				const int left_gap   = gap_size;
				const int top_gap    = (i == m->nmaster + 1) ? gap_size : top_left_half_gap;
				const int right_gap  = bottom_right_half_gap;
				const int bottom_gap = (i == n - 1 || i == n - 2) ? gap_size : bottom_right_half_gap;

				resize(
					c,
					m->wx + left_gap,
					m->wy + ety + top_gap,
					tw - 2 * border_width - left_gap - right_gap,
					h - 2 * border_width - top_gap - bottom_gap,
					border_width,
					0
				);

				ety += HEIGHT(c) + top_gap + bottom_gap;
			} else {
				const unsigned int h = (m->wh - oty) / ((1 + n - i) / 2);

				const int left_gap   = (m->nmaster == 0 && n == 1) ? gap_size : top_left_half_gap;
				const int top_gap    = i == m->nmaster ? gap_size : top_left_half_gap;
				const int right_gap  = gap_size;
				const int bottom_gap = (i == n - 1 || i == n - 2) ? gap_size : bottom_right_half_gap;

				resize(
					c,
					m->wx + mx + mw + left_gap,
					m->wy + oty + top_gap,
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

void
monocle(Monitor *m)
{
	const SettingsForSingleWindow gap_for_single_window = settings_get_gap_for_single_window();
	const SettingsForSingleWindow border_for_single_window = settings_get_border_for_single_window();

	const int gap_size = (
		gap_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_ALWAYS
		||
		(
			gap_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN
			&&
			!m->sel->isfullscreen
		)
	) ? settings_get_gap_size() : 0;

	const int border_width = (
		border_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_ALWAYS
		||
		(
			border_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN
			&&
			!m->sel->isfullscreen
		)
	) ? settings_get_border_width() : 0;

	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		resize(
			c,
			m->wx + gap_size,
			m->wy + gap_size,
			m->ww - 2 * border_width - 2 * gap_size,
			m->wh - 2 * border_width - 2 * gap_size,
			border_width,
			0
		);
	}
}

void
tile(Monitor *m)
{
	unsigned int n = 0;
	for (Client *c = nexttiled(m->clients); c; c = nexttiled(c->next), ++n);
	if (n == 0) return;

	const SettingsForSingleWindow gap_for_single_window = settings_get_gap_for_single_window();
	const SettingsForSingleWindow border_for_single_window = settings_get_border_for_single_window();

	const int gap_size = (
		n > 1
		||
		gap_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_ALWAYS
		||
		(
			gap_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN
			&&
			!m->sel->isfullscreen
		)
	) ? settings_get_gap_size() : 0;

	const int border_width = (
		n > 1
		||
		border_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_ALWAYS
		||
		(
			border_for_single_window == SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN
			&&
			!m->sel->isfullscreen
		)
	) ? settings_get_border_width() : 0;

	const int top_left_half_gap = gap_size / 2;
	const int bottom_right_half_gap = gap_size - top_left_half_gap;

	const unsigned int mw = n > m->nmaster ? (m->nmaster ? m->ww * m->mfact : 0) : m->ww;

	Client *c = nexttiled(m->clients);

	for (unsigned int i = 0, my = 0, ty = 0; c; c = nexttiled(c->next), ++i) {
		if (i < m->nmaster) {
			const unsigned int h = (m->wh - my) / (MIN(n, m->nmaster) - i);

			const unsigned int left_gap   = gap_size;
			const unsigned int top_gap    = i == 0 ? gap_size : top_left_half_gap;
			const unsigned int right_gap  = n <= m->nmaster ? gap_size : bottom_right_half_gap;
			const unsigned int bottom_gap = (i == m->nmaster - 1 || i == n - 1) ? gap_size : bottom_right_half_gap;

			resize(
				c,
				m->wx + left_gap,
				m->wy + my + top_gap,
				mw - 2 * border_width - left_gap - right_gap,
				h - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			if (my + HEIGHT(c) < m->wh) {
				my += HEIGHT(c) + top_gap + bottom_gap;
			}
		} else {
			const unsigned int h = (m->wh - ty) / (n - i);

			const unsigned int left_gap   = m->nmaster == 0 ? gap_size : top_left_half_gap;
			const unsigned int top_gap    = i == m->nmaster ? gap_size : top_left_half_gap;
			const unsigned int right_gap  = gap_size;
			const unsigned int bottom_gap = (i == n - 1) ? gap_size : bottom_right_half_gap;

			resize(
				c,
				m->wx + mw + left_gap,
				m->wy + ty + top_gap,
				m->ww - mw - 2 * border_width - left_gap - right_gap,
				h - 2 * border_width - top_gap - bottom_gap,
				border_width,
				0
			);

			if (ty + HEIGHT(c) < m->wh) {
				ty += HEIGHT(c) + top_gap + bottom_gap;
			}
		}
	}
}
