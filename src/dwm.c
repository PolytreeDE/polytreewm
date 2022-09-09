#include "dwm.h"

#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "settings.h"
#include "spawn.h"
#include "state.h"
#include "unit.h"
#include "util.h"
#include "xbase.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#ifdef ENABLE_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif // ENABLE_XINERAMA

// TODO: Include necessary headers in this header.
#include "drw.h"

/**********
 * macros *
 **********/

#define BUTTONMASK   (ButtonPressMask | ButtonReleaseMask)
#define ISVISIBLE(C) (true)
#define LENGTH(X)    (sizeof(X) / sizeof(X[0]))
#define MOUSEMASK    (BUTTONMASK | PointerMotionMask)

#define CLEANMASK(mask) (                            \
	(mask) &                                         \
	~(numlockmask | LockMask) &                      \
	(ShiftMask | ControlMask | Mod1Mask | Mod2Mask | \
		Mod3Mask | Mod4Mask | Mod5Mask)              \
)

#define INTERSECT(x,y,w,h,basic_geom) ( \
	MAX(                                \
		0,                              \
		MIN(                            \
			(x) + (w),                  \
			basic_geom.position.x       \
			+                           \
			basic_geom.sizes.w          \
		)                               \
		-                               \
		MAX(                            \
			(x),                        \
			basic_geom.position.x       \
		)                               \
	)                                   \
	*                                   \
	MAX(                                \
		0,                              \
		MIN(                            \
			(y) + (h),                  \
			basic_geom.position.y       \
			+                           \
			basic_geom.sizes.h          \
		)                               \
		-                               \
		MAX(                            \
			(y),                        \
			basic_geom.position.y       \
		)                               \
	)                                   \
)

/*********
 * types *
 *********/

enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel }; /* color schemes */
enum { ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

typedef struct Monitor Monitor;
typedef struct Client Client;

typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

typedef struct {
	unsigned int click;
	unsigned int mask;
	unsigned int button;
	void (*func)(const Arg *arg);
	const Arg arg;
} Button;

struct Client {
	struct ClientState state;
	struct ClientSizeHints size_hints;

	Client *next;
	Client *snext;
	Monitor *mon;
	Window x_window;
};

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

typedef struct {
	void (*arrange)(Monitor *);
} Layout;

struct Monitor {
	struct BasicGeom screen_geom;
	struct BasicGeom window_area_geom;
	Unit unit;

	int nmaster;
	int num;
	unsigned int sellt;
	Client *clients;
	Client *sel;
	Client *stack;
	Monitor *next;
	const Layout *lt[2];
};

/*************************
 * function declarations *
 *************************/

static int applysizehints(Client *c, WinGeom win_geom, int interact);
static void arrange(Monitor *m);
static void arrangemon(Monitor *m);
static void attach(Client *c);
static void attachstack(Client *c);
static void configure(Client *c);
static void detach(Client *c);
static void detachstack(Client *c);
static Monitor *dirtomon(int dir);
static void focus(Client *c);
static Atom getatomprop(Client *c, Atom prop);
static int getrootptr(int *x, int *y);
static long getstate(Window w);
static int gettextprop(Window w, Atom atom, char *text, unsigned int size);
static void grabbuttons(Client *c, int focused);
static void grabkeys();
static int isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info);
static void manage(Window w, XWindowAttributes *wa);
static void managepolybar(Window w, XWindowAttributes *wa);
static Monitor *monitor_create();
static void monitor_destroy(Monitor *mon);
static Client *nexttiled(Client *c);
static void pop(Client *);
static Monitor *recttomon(int x, int y, int w, int h);
static void resize(Client *c, struct WinGeom win_geom, int interact);
static void resizeclient(Client *c, struct WinGeom win_geom);
static void restack(Monitor *m);
static void run();
static void scan();
static void scheme_create();
static void scheme_destroy();
static int sendevent(Client *c, Atom proto);
static void sendmon(Client *c, Monitor *m);
static void setclientstate(Client *c, long state);
static void setfocus(Client *c);
static void setfullscreen(Client *c, int fullscreen);
static void seturgent(Client *c, bool is_urgent);
static void showhide(Client *c);
static void unfocus(Client *c, int setfocus);
static void unmanage(Client *c, int destroyed);
static void updateclientlist();
static int updategeom();
static void updatenumlockmask();
static void updatesizehints(Client *c);
static void updatetitle(Client *c);
static void updatewindowtype(Client *c);
static void updatewmhints(Client *c);
static Client *wintoclient(Window w);
static Monitor *wintomon(Window w);

extern const Layout layouts[];

#include "dwm/bar.h"
#include "dwm/handlers.h"
#include "dwm/interaction.h"
#include "dwm/layouts.h"
#include "dwm/wmcheckwin.h"
#include "dwm/xerror.h"

/*************
 * variables *
 *************/

static Xbase xbase = NULL;

static Unit global_unit = NULL;

static const char broken[] = "broken";
static unsigned int numlockmask = 0;
static int running = 1;
static Cur *cursor[CurLast];
static Clr **scheme;
static Drw *drw;
static Monitor *mons, *selmon;

static void (*handler[LASTEvent])(XEvent*) = {
	[ButtonPress] = on_button_press,
	[ClientMessage] = on_client_message,
	[ConfigureRequest] = on_configure_request,
	[ConfigureNotify] = on_configure_notify,
	[DestroyNotify] = on_destroy_notify,
	[FocusIn] = on_focus_in,
	[KeyPress] = on_key_press,
	[MappingNotify] = on_mapping_notify,
	[MapRequest] = on_map_request,
	[PropertyNotify] = on_property_notify,
	[UnmapNotify] = on_unmap_notify
};

/***************************************************************
 * configuration, allows nested code to access above variables *
 ***************************************************************/

static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  "#d9b01c" },
};

const Layout layouts[] = {
	/* arrange function */
	{ monocle }, /* first entry is default */
	{ NULL },    /* no layout function means floating behavior */
	{ tile },
	{ horizontile },
	{ centeredmaster },
};

/************************************
 * Private function implementations *
 ************************************/

#include "dwm/bar.c"
#include "dwm/handlers.c"
#include "dwm/interaction.c"
#include "dwm/layouts.c"
#include "dwm/wmcheckwin.c"
#include "dwm/xerror.c"

/***********************************
 * Public function implementations *
 ***********************************/

int dwm_main(const char *const new_program_title)
{
	xbase = xbase_new(new_program_title, xerror);

	if (!(global_unit = unit_new(UNIT_GLOBAL, NULL))) {
		fatal("cannot create global unit");
	}

	// Setup start

	XSetWindowAttributes wa;

	drw = drw_create(
		xbase->x_display,
		xbase->x_screen,
		xbase->x_root,
		xbase->screen_sizes.w,
		xbase->screen_sizes.h
	);

	updategeom();
	/* init cursors */
	cursor[CurNormal] = drw_cur_create(drw, XC_left_ptr);
	cursor[CurResize] = drw_cur_create(drw, XC_sizing);
	cursor[CurMove] = drw_cur_create(drw, XC_fleur);
	/* init appearance */
	scheme_create();

	/* supporting window for NetWMCheck */
	wmcheckwin_create();

	/* EWMH support per view */
	XChangeProperty(
		xbase->x_display,
		xbase->x_root,
		xbase->atoms->netatom[NetSupported],
		XA_ATOM,
		32,
		PropModeReplace,
		(unsigned char*)xbase->atoms->netatom,
		NetLast
	);
	XDeleteProperty(
		xbase->x_display,
		xbase->x_root,
		xbase->atoms->netatom[NetClientList]
	);

	/* select events */
	wa.cursor = cursor[CurNormal]->cursor;
	wa.event_mask =
		SubstructureRedirectMask | SubstructureNotifyMask | ButtonPressMask |
		PointerMotionMask | EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask | PropertyChangeMask;
	XChangeWindowAttributes(
		xbase->x_display,
		xbase->x_root,
		CWEventMask | CWCursor,
		&wa
	);
	XSelectInput(xbase->x_display, xbase->x_root, wa.event_mask);
	grabkeys();
	focus(NULL);

	// Setup end

	scan();
	run();

	// Cleanup start

	Layout foo = { NULL };
	selmon->lt[selmon->sellt] = &foo;

	for (Monitor *m = mons; m; m = m->next) {
		while (m->stack) {
			unmanage(m->stack, 0);
		}
	}

	XUngrabKey(xbase->x_display, AnyKey, AnyModifier, xbase->x_root);

	while (mons) {
		monitor_destroy(mons);
	}

	for (size_t i = 0; i < CurLast; i++) {
		drw_cur_free(drw, cursor[i]);
	}

	scheme_destroy();

	wmcheckwin_destroy();
	drw_free(drw);
	XSync(xbase->x_display, False);
	XSetInputFocus(
		xbase->x_display,
		PointerRoot,
		RevertToPointerRoot,
		CurrentTime
	);
	XDeleteProperty(
		xbase->x_display,
		xbase->x_root,
		xbase->atoms->netatom[NetActiveWindow]
	);

	// Cleanup end

	UNIT_DELETE(global_unit);
	XBASE_DELETE(xbase);

	return EXIT_SUCCESS;
}

/************************************
 * Private function implementations *
 ************************************/

int applysizehints(
	Client *c,
	WinGeom win_geom,
	int interact
) {
	int *const x = &win_geom->basic.position.x;
	int *const y = &win_geom->basic.position.y;
	int *const w = &win_geom->basic.sizes.w;
	int *const h = &win_geom->basic.sizes.h;
	int const bw = win_geom->border_width;

	Monitor *m = c->mon;

	/* set minimum possible */
	*w = MAX(1, *w);
	*h = MAX(1, *h);

	if (interact) {
		if (*x > xbase->screen_sizes.w) {
			*x =
				xbase->screen_sizes.w
				-
				win_geom_total_width(&c->state.geom);
		}
		if (*y > xbase->screen_sizes.h) {
			*y =
				xbase->screen_sizes.h
				-
				win_geom_total_height(&c->state.geom);
		}
		if (*x + *w + 2 * bw < 0) {
			*x = 0;
		}
		if (*y + *h + 2 * bw < 0) {
			*y = 0;
		}
	} else {
		if (
			*x
			>=
			m->window_area_geom.position.x + m->window_area_geom.sizes.w
		) {
			*x =
				m->window_area_geom.position.x
				+
				m->window_area_geom.sizes.w
				-
				win_geom_total_width(&c->state.geom);
		}
		if (
			*y
			>=
			m->window_area_geom.position.y + m->window_area_geom.sizes.h
		) {
			*y =
				m->window_area_geom.position.y
				+
				m->window_area_geom.sizes.h
				-
				win_geom_total_height(&c->state.geom);
		}
		if (*x + *w + 2 * bw <= m->window_area_geom.position.x) {
			*x = m->window_area_geom.position.x;
		}
		if (*y + *h + 2 * bw <= m->window_area_geom.position.y) {
			*y = m->window_area_geom.position.y;
		}
	}

	if (
		c->state.is_floating
		||
		(
			settings_get_respect_resize_hints_in_floating_layout()
			&&
			!c->mon->lt[c->mon->sellt]->arrange
		)
	) {
		/* see last two sentences in ICCCM 4.1.2.3 */
		const int baseismin =
			c->size_hints.basew == c->size_hints.minw
			&&
			c->size_hints.baseh == c->size_hints.minh;

		/* temporarily remove base dimensions */
		if (!baseismin) {
			*w -= c->size_hints.basew;
			*h -= c->size_hints.baseh;
		}

		/* adjust for aspect limits */
		if (c->size_hints.mina > 0 && c->size_hints.maxa > 0) {
			if (c->size_hints.maxa < (float)*w / *h) {
				*w = *h * c->size_hints.maxa + 0.5;
			} else if (c->size_hints.mina < (float)*h / *w) {
				*h = *w * c->size_hints.mina + 0.5;
			}
		}

		/* increment calculation requires this */
		if (baseismin) {
			*w -= c->size_hints.basew;
			*h -= c->size_hints.baseh;
		}

		/* adjust for increment value */
		if (c->size_hints.incw) {
			*w -= *w % c->size_hints.incw;
		}
		if (c->size_hints.inch) {
			*h -= *h % c->size_hints.inch;
		}

		/* restore base dimensions */
		*w = MAX(*w + c->size_hints.basew, c->size_hints.minw);
		*h = MAX(*h + c->size_hints.baseh, c->size_hints.minh);

		if (c->size_hints.maxw) {
			*w = MIN(*w, c->size_hints.maxw);
		}
		if (c->size_hints.maxh) {
			*h = MIN(*h, c->size_hints.maxh);
		}
	}

	return (
		*x != c->state.geom.basic.position.x
		||
		*y != c->state.geom.basic.position.y
		||
		*w != c->state.geom.basic.sizes.w
		||
		*h != c->state.geom.basic.sizes.h
		||
		bw != c->state.geom.border_width
	);
}

void arrange(Monitor *m)
{
	if (m) {
		showhide(m->stack);
	} else {
		for (m = mons; m; m = m->next) {
			showhide(m->stack);
		}
	}

	if (m) {
		arrangemon(m);
		restack(m);
	} else {
		for (m = mons; m; m = m->next) {
			arrangemon(m);
		}
	}
}

void arrangemon(Monitor *m)
{
	unsigned int visible_clients = 0;

	for (const Client *client = m->clients; client; client = client->next) {
		if (ISVISIBLE(client)) {
			++visible_clients;
		}
	}

	if (m->lt[m->sellt]->arrange) {
		m->lt[m->sellt]->arrange(m);
	} else {
		floating(selmon);
	}
}

void attach(Client *c)
{
	c->next = c->mon->clients;
	c->mon->clients = c;
}

void attachstack(Client *c)
{
	c->snext = c->mon->stack;
	c->mon->stack = c;
}

void configure(Client *c)
{
	XConfigureEvent ce = {
		.type = ConfigureNotify,
		.serial = 0,
		.send_event = False,
		.display = xbase->x_display,
		.event = c->x_window,
		.window = c->x_window,
		.x = c->state.geom.basic.position.x,
		.y = c->state.geom.basic.position.y,
		.width = c->state.geom.basic.sizes.w,
		.height = c->state.geom.basic.sizes.h,
		.border_width = c->state.geom.border_width,
		.above = None,
		.override_redirect = False,
	};

	XSendEvent(
		xbase->x_display,
		c->x_window,
		False,
		StructureNotifyMask,
		(XEvent*)&ce
	);
}

void detach(Client *c)
{
	Client **tc;

	for (tc = &c->mon->clients; *tc && *tc != c; tc = &(*tc)->next);
	*tc = c->next;
}

void detachstack(Client *c)
{
	Client **tc, *t;

	for (tc = &c->mon->stack; *tc && *tc != c; tc = &(*tc)->snext);
	*tc = c->snext;

	if (c == c->mon->sel) {
		for (t = c->mon->stack; t && !ISVISIBLE(t); t = t->snext);
		c->mon->sel = t;
	}
}

Monitor *dirtomon(int dir)
{
	Monitor *m = NULL;

	if (dir > 0) {
		if (!(m = selmon->next))
			m = mons;
	} else if (selmon == mons)
		for (m = mons; m->next; m = m->next);
	else
		for (m = mons; m->next != selmon; m = m->next);
	return m;
}

void focus(Client *c)
{
	if (!c || !ISVISIBLE(c))
		for (c = selmon->stack; c && !ISVISIBLE(c); c = c->snext);
	if (selmon->sel && selmon->sel != c)
		unfocus(selmon->sel, 0);
	if (c) {
		if (c->mon != selmon)
			selmon = c->mon;
		if (c->state.is_urgent)
			seturgent(c, false);
		detachstack(c);
		attachstack(c);
		grabbuttons(c, 1);
		XSetWindowBorder(
			xbase->x_display,
			c->x_window,
			scheme[SchemeSel][ColBorder].pixel
		);
		setfocus(c);
	} else {
		XSetInputFocus(
			xbase->x_display,
			xbase->x_root,
			RevertToPointerRoot,
			CurrentTime
		);
		XDeleteProperty(
			xbase->x_display,
			xbase->x_root,
			xbase->atoms->netatom[NetActiveWindow]
		);
	}
	selmon->sel = c;
}

Atom getatomprop(Client *c, Atom prop)
{
	Atom atom = None;

	Atom da = None;
	int di = 0;
	unsigned long dl = 0;
	unsigned char *p = NULL;

	if (
		XGetWindowProperty(
			xbase->x_display,
			c->x_window,
			prop,
			0L,
			sizeof(atom),
			False,
			XA_ATOM,
			&da,
			&di,
			&dl,
			&dl,
			&p
		) == Success && p
	) {
		atom = *(Atom*)p;
		XFree(p);
	}

	return atom;
}

int getrootptr(int *x, int *y)
{
	int di;
	unsigned int dui;
	Window dummy;

	return XQueryPointer(
		xbase->x_display,
		xbase->x_root,
		&dummy,
		&dummy,
		x,
		y,
		&di,
		&di,
		&dui
	);
}

long getstate(Window w)
{
	int format;
	long result = -1;
	unsigned char *p = NULL;
	unsigned long n, extra;
	Atom real;

	if (
		XGetWindowProperty(
			xbase->x_display,
			w,
			xbase->atoms->wmatom[WMState],
			0L,
			2L,
			False,
			xbase->atoms->wmatom[WMState],
			&real,
			&format,
			&n,
			&extra,
			(unsigned char **)&p
		) != Success
	) {
		return -1;
	}

	if (n != 0) {
		result = *p;
	}

	XFree(p);
	return result;
}

int gettextprop(Window w, Atom atom, char *text, unsigned int size)
{
	char **list = NULL;
	int n;
	XTextProperty name;

	if (!text || size == 0) {
		return 0;
	}

	text[0] = '\0';

	if (!XGetTextProperty(xbase->x_display, w, &name, atom) || !name.nitems) {
		return 0;
	}

	if (name.encoding == XA_STRING) {
		strncpy(text, (char *)name.value, size - 1);
	} else {
		if (
			(
				XmbTextPropertyToTextList(xbase->x_display, &name, &list, &n)
				>=
				Success
			)
			&&
			n > 0
			&&
			*list
		) {
			strncpy(text, *list, size - 1);
			XFreeStringList(list);
		}
	}
	text[size - 1] = '\0';
	XFree(name.value);
	return 1;
}

void grabbuttons(Client *c, int focused)
{
	updatenumlockmask();
	{
		unsigned int i, j;
		unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
		XUngrabButton(xbase->x_display, AnyButton, AnyModifier, c->x_window);

		if (!focused) {
			XGrabButton(
				xbase->x_display,
				AnyButton,
				AnyModifier,
				c->x_window,
				False,
				BUTTONMASK,
				GrabModeSync,
				GrabModeSync,
				None,
				None
			);
		}

		for (i = 0; i < LENGTH(buttons); i++) {
			if (buttons[i].click == ClkClientWin) {
				for (j = 0; j < LENGTH(modifiers); j++) {
					XGrabButton(
						xbase->x_display,
						buttons[i].button,
						buttons[i].mask | modifiers[j],
						c->x_window,
						False,
						BUTTONMASK,
						GrabModeAsync,
						GrabModeSync,
						None,
						None
					);
				}
			}
		}
	}
}

void grabkeys()
{
	updatenumlockmask();
	{
		unsigned int i, j;
		unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
		KeyCode code;

		XUngrabKey(xbase->x_display, AnyKey, AnyModifier, xbase->x_root);

		for (i = 0; i < LENGTH(keys); i++) {
			if ((code = XKeysymToKeycode(xbase->x_display, keys[i].keysym))) {
				for (j = 0; j < LENGTH(modifiers); j++) {
					XGrabKey(
						xbase->x_display,
						code,
						keys[i].mod | modifiers[j],
						xbase->x_root,
						True,
						GrabModeAsync,
						GrabModeAsync
					);
				}
			}
		}
	}
}

#ifdef ENABLE_XINERAMA
int isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info)
{
	while (n--)
		if (unique[n].x_org == info->x_org && unique[n].y_org == info->y_org
		&& unique[n].width == info->width && unique[n].height == info->height)
			return 0;
	return 1;
}
#endif /* ENABLE_XINERAMA */

void manage(Window w, XWindowAttributes *wa)
{
	Client *const c = ecalloc(1, sizeof(Client));

	client_state_init(&c->state);

	c->x_window = w;
	c->state.geom.basic.position.x = wa->x;
	c->state.geom.basic.position.y = wa->y;
	c->state.geom.basic.sizes.w = wa->width;
	c->state.geom.basic.sizes.h = wa->height;
	c->state.is_floating = false;

	updatetitle(c);

	Window trans = None;

	{
		Client *t = NULL;

		if (
			XGetTransientForHint(xbase->x_display, w, &trans)
			&&
			(t = wintoclient(trans))
		) {
			c->mon = t->mon;
		} else {
			c->mon = selmon;
		}
	}

	win_geom_adjust_to_boundary(
		&c->state.geom,
		&c->mon->screen_geom
	);

	c->state.geom.border_width = settings_get_border_width();

	{
		XWindowChanges wc;
		wc.border_width = c->state.geom.border_width;
		XConfigureWindow(xbase->x_display, w, CWBorderWidth, &wc);
	}

	XSetWindowBorder(xbase->x_display, w, scheme[SchemeNorm][ColBorder].pixel);

	configure(c); /* propagates border_width, if size doesn't change */
	updatewindowtype(c);
	updatesizehints(c);
	updatewmhints(c);

	{
		const int total_width = win_geom_total_width(&c->state.geom);
		const int total_height =
			win_geom_total_height(&c->state.geom);

		c->state.geom.basic.position.x =
			c->mon->screen_geom.position.x
			+
			(c->mon->screen_geom.sizes.w - total_width) / 2;
		c->state.geom.basic.position.y =
			c->mon->screen_geom.position.y
			+
			(c->mon->screen_geom.sizes.h - total_height) / 2;
	}

	XSelectInput(
		xbase->x_display,
		w,
		EnterWindowMask |
			FocusChangeMask |
			PropertyChangeMask |
			StructureNotifyMask
	);

	grabbuttons(c, 0);

	if (!c->state.is_floating) {
		c->state.is_floating = trans != None || c->state.is_fixed;
	}

	if (c->state.is_floating) {
		XRaiseWindow(xbase->x_display, c->x_window);
	}

	attach(c);
	attachstack(c);

	XChangeProperty(
		xbase->x_display,
		xbase->x_root,
		xbase->atoms->netatom[NetClientList],
		XA_WINDOW,
		32,
		PropModeAppend,
		(unsigned char*)&(c->x_window),
		1
	);

	/* some windows require this */
	XMoveResizeWindow(
		xbase->x_display,
		c->x_window,
		c->state.geom.basic.position.x + 2 * xbase->screen_sizes.w,
		c->state.geom.basic.position.y,
		c->state.geom.basic.sizes.w,
		c->state.geom.basic.sizes.h
	);

	setclientstate(c, NormalState);

	if (c->mon == selmon) {
		unfocus(selmon->sel, 0);
	}

	c->mon->sel = c;

	arrange(c->mon);
	XMapWindow(xbase->x_display, c->x_window);
	focus(NULL);
}

void managepolybar(Window w, XWindowAttributes *wa)
{
	Monitor *m = recttomon(wa->x, wa->y, wa->width, wa->height);
	if (!m) return;

	XSelectInput(
		xbase->x_display,
		w,
		EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask
	);

	XMoveResizeWindow(
		xbase->x_display,
		w,
		wa->x,
		wa->y,
		wa->width,
		wa->height
	);

	XMapWindow(xbase->x_display, w);

	XChangeProperty(
		xbase->x_display,
		xbase->x_root,
		xbase->atoms->netatom[NetClientList],
		XA_WINDOW,
		32,
		PropModeAppend,
		(unsigned char*)&w,
		1
	);
}

Monitor *monitor_create()
{
	Monitor *const m = ecalloc(1, sizeof(Monitor));

	m->screen_geom      = basic_geom_create();
	m->window_area_geom = basic_geom_create();

	if (!m) goto fail_without_mon;

	if (!(m->unit = unit_new(UNIT_MONITOR, global_unit))) {
		goto fail_without_unit;
	}

	m->nmaster = settings_get_default_clients_in_master();
	m->lt[0] = &layouts[0];
	m->lt[1] = &layouts[1 % LENGTH(layouts)];

	return m;

fail_without_unit:
	free(m);
fail_without_mon:
	return NULL;
}

void monitor_destroy(Monitor *mon)
{
	if (mon == mons) {
		mons = mons->next;
	} else {
		Monitor *m;
		for (m = mons; m && m->next != mon; m = m->next);
		m->next = mon->next;
	}

	UNIT_DELETE(mon->unit);
	free(mon);
}

Client *nexttiled(Client *c)
{
	for (; c && (c->state.is_floating || !ISVISIBLE(c)); c = c->next);
	return c;
}

void pop(Client *c)
{
	detach(c);
	attach(c);
	focus(c);
	arrange(c->mon);
}

Monitor *recttomon(int x, int y, int w, int h)
{
	Monitor *m, *r = selmon;
	int a, area = 0;

	for (m = mons; m; m = m->next)
		if ((a = INTERSECT(x, y, w, h, m->window_area_geom)) > area) {
			area = a;
			r = m;
		}
	return r;
}

void resize(Client *c, struct WinGeom win_geom, int interact)
{
	if (applysizehints(c, &win_geom, interact)) {
		resizeclient(c, win_geom);
	}
}

void resizeclient(Client *c, const struct WinGeom win_geom)
{
	c->state.geom = win_geom;

	XWindowChanges wc = { 0 };
	win_geom_convert_to_x_window_changes(&win_geom, &wc);

	XConfigureWindow(
		xbase->x_display,
		c->x_window,
		CWX | CWY | CWWidth | CWHeight | CWBorderWidth,
		&wc
	);

	configure(c);
	XSync(xbase->x_display, False);
}

void restack(Monitor *m)
{
	Client *c;
	XEvent ev;
	XWindowChanges wc;

	if (!m->sel) return;

	if (m->sel->state.is_floating || !m->lt[m->sellt]->arrange) {
		XRaiseWindow(xbase->x_display, m->sel->x_window);
	}

	if (m->lt[m->sellt]->arrange) {
		wc.stack_mode = Below;
		// TODO: Learn what is sibling and what
		// is the following line responsible for.
		// wc.sibling = m->bar->barwin;
		for (c = m->stack; c; c = c->snext) {
			if (!c->state.is_floating && ISVISIBLE(c)) {
				XConfigureWindow(
					xbase->x_display,
					c->x_window,
					CWSibling | CWStackMode,
					&wc
				);
				wc.sibling = c->x_window;
			}
		}
	}
	XSync(xbase->x_display, False);
	while (XCheckMaskEvent(xbase->x_display, EnterWindowMask, &ev));
}

void run()
{
	XEvent ev;
	/* main event loop */
	XSync(xbase->x_display, False);
	while (running && !XNextEvent(xbase->x_display, &ev)) {
		if (handler[ev.type]) {
			handler[ev.type](&ev); /* call handler */
		}
	}
}

void scan()
{
	unsigned int i, num;
	Window d1, d2, *wins = NULL;
	XWindowAttributes wa;

	if (XQueryTree(xbase->x_display, xbase->x_root, &d1, &d2, &wins, &num)) {
		for (i = 0; i < num; i++) {
			if (
				!XGetWindowAttributes(xbase->x_display, wins[i], &wa)
				||
				wa.override_redirect
				||
				XGetTransientForHint(xbase->x_display, wins[i], &d1)
			) {
				continue;
			}

			if (winpolybar(wins[i])) {
				managepolybar(wins[i], &wa);
			} else if (
				wa.map_state == IsViewable
				||
				getstate(wins[i]) == IconicState
			) {
				manage(wins[i], &wa);
			}
		}
		for (i = 0; i < num; i++) { /* now the transients */
			if (!XGetWindowAttributes(xbase->x_display, wins[i], &wa)) {
				continue;
			}
			if (winpolybar(wins[i])) {
				// do nothing
			} else if (
				XGetTransientForHint(xbase->x_display, wins[i], &d1)
				&&
				(wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
			) {
				manage(wins[i], &wa);
			}
		}
		if (wins)
			XFree(wins);
	}
}

void scheme_create()
{
	scheme = ecalloc(LENGTH(colors), sizeof(Clr*));

	for (unsigned int i = 0; i < LENGTH(colors); ++i) {
		scheme[i] = drw_scm_create(drw, colors[i], 3);
	}
}

void scheme_destroy()
{
	for (size_t i = 0; i < LENGTH(colors); ++i) {
		free(scheme[i]);
	}
}

void sendmon(Client *c, Monitor *m)
{
	if (c->mon == m)
		return;
	unfocus(c, 1);
	detach(c);
	detachstack(c);
	c->mon = m;
	attach(c);
	attachstack(c);
	focus(NULL);
	arrange(NULL);
}

void setclientstate(Client *c, long state)
{
	long data[] = { state, None };

	XChangeProperty(
		xbase->x_display,
		c->x_window,
		xbase->atoms->wmatom[WMState],
		xbase->atoms->wmatom[WMState],
		32,
		PropModeReplace,
		(unsigned char*)data,
		2
	);
}

int sendevent(Client *c, Atom proto)
{
	int n;
	Atom *protocols;
	int exists = 0;
	XEvent ev;

	if (XGetWMProtocols(xbase->x_display, c->x_window, &protocols, &n)) {
		while (!exists && n--) {
			exists = protocols[n] == proto;
		}
		XFree(protocols);
	}

	if (exists) {
		ev.type = ClientMessage;
		ev.xclient.window = c->x_window;
		ev.xclient.message_type = xbase->atoms->wmatom[WMProtocols];
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = proto;
		ev.xclient.data.l[1] = CurrentTime;
		XSendEvent(xbase->x_display, c->x_window, False, NoEventMask, &ev);
	}

	return exists;
}

void setfocus(Client *c)
{
	if (!c->state.never_focus) {
		XSetInputFocus(
			xbase->x_display,
			c->x_window,
			RevertToPointerRoot,
			CurrentTime
		);
		XChangeProperty(
			xbase->x_display,
			xbase->x_root,
			xbase->atoms->netatom[NetActiveWindow],
			XA_WINDOW,
			32,
			PropModeReplace,
			(unsigned char*)&(c->x_window),
			1
		);
	}

	sendevent(c, xbase->atoms->wmatom[WMTakeFocus]);
}

void setfullscreen(Client *c, int fullscreen)
{
	if (fullscreen && !c->state.is_fullscreen) {
		XChangeProperty(
			xbase->x_display,
			c->x_window,
			xbase->atoms->netatom[NetWMState],
			XA_ATOM,
			32,
			PropModeReplace,
			(unsigned char*)&xbase->atoms->netatom[NetWMFullscreen],
			1
		);
		c->state.is_fullscreen = true;
		// We have to rearrange because borders and gaps may have changed.
		arrange(c->mon);
	} else if (!fullscreen && c->state.is_fullscreen){
		XChangeProperty(
			xbase->x_display,
			c->x_window,
			xbase->atoms->netatom[NetWMState],
			XA_ATOM,
			32,
			PropModeReplace,
			(unsigned char*)0,
			0
		);
		c->state.is_fullscreen = false;
		// We have to rearrange because borders and gaps may have changed.
		arrange(c->mon);
	}
}

void seturgent(Client *c, bool is_urgent)
{
	XWMHints *wmh;

	c->state.is_urgent = is_urgent;
	if (!(wmh = XGetWMHints(xbase->x_display, c->x_window))) return;

	wmh->flags =
		is_urgent ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
	XSetWMHints(xbase->x_display, c->x_window, wmh);
	XFree(wmh);
}

void showhide(Client *c)
{
	if (!c)
		return;
	if (ISVISIBLE(c)) {
		/* show clients top down */
		XMoveWindow(
			xbase->x_display,
			c->x_window,
			c->state.geom.basic.position.x,
			c->state.geom.basic.position.y
		);

		if (!c->mon->lt[c->mon->sellt]->arrange || c->state.is_floating) {
			resize(c, c->state.geom, 0);
		}
		showhide(c->snext);
	} else {
		/* hide clients bottom up */
		showhide(c->snext);
		XMoveWindow(
			xbase->x_display,
			c->x_window,
			win_geom_total_width(&c->state.geom) * -2,
			c->state.geom.basic.position.y
		);
	}
}

void unfocus(Client *c, int setfocus)
{
	if (!c)
		return;
	grabbuttons(c, 0);
	XSetWindowBorder(
		xbase->x_display,
		c->x_window,
		scheme[SchemeNorm][ColBorder].pixel
	);
	if (setfocus) {
		XSetInputFocus(
			xbase->x_display,
			xbase->x_root,
			RevertToPointerRoot,
			CurrentTime
		);
		XDeleteProperty(
			xbase->x_display,
			xbase->x_root,
			xbase->atoms->netatom[NetActiveWindow]
		);
	}
}

void unmanage(Client *c, int destroyed)
{
	Monitor *m = c->mon;
	XWindowChanges wc;

	detach(c);
	detachstack(c);
	if (!destroyed) {
		wc.border_width = 0;
		XGrabServer(xbase->x_display); /* avoid race conditions */
		XSetErrorHandler(xerrordummy);
		XConfigureWindow(xbase->x_display, c->x_window, CWBorderWidth, &wc); /* remove border */
		XUngrabButton(xbase->x_display, AnyButton, AnyModifier, c->x_window);
		setclientstate(c, WithdrawnState);
		XSync(xbase->x_display, False);
		XSetErrorHandler(xerror);
		XUngrabServer(xbase->x_display);
	}
	free(c);
	focus(NULL);
	updateclientlist();
	arrange(m);
}

void updateclientlist()
{
	XDeleteProperty(
		xbase->x_display,
		xbase->x_root,
		xbase->atoms->netatom[NetClientList]
	);

	for (Monitor *m = mons; m; m = m->next) {
		for (Client *c = m->clients; c; c = c->next) {
			XChangeProperty(
				xbase->x_display,
				xbase->x_root,
				xbase->atoms->netatom[NetClientList],
				XA_WINDOW,
				32,
				PropModeAppend,
				(unsigned char*)&(c->x_window),
				1
			);
		}
	}
}

int updategeom()
{
	int dirty = 0;

#ifdef ENABLE_XINERAMA
	if (XineramaIsActive(xbase->x_display)) {
		int i, j, n, nn;
		Client *c;
		Monitor *m;
		XineramaScreenInfo *info = XineramaQueryScreens(xbase->x_display, &nn);
		XineramaScreenInfo *unique = NULL;

		for (n = 0, m = mons; m; m = m->next, n++);
		/* only consider unique geometries as separate screens */
		unique = ecalloc(nn, sizeof(XineramaScreenInfo));
		for (i = 0, j = 0; i < nn; i++)
			if (isuniquegeom(unique, j, &info[i]))
				memcpy(&unique[j++], &info[i], sizeof(XineramaScreenInfo));
		XFree(info);
		nn = j;
		if (n <= nn) { /* new monitors available */
			for (i = 0; i < (nn - n); i++) {
				for (m = mons; m && m->next; m = m->next);
				if (m)
					m->next = monitor_create();
				else
					mons = monitor_create();
			}
			for (i = 0, m = mons; i < nn && m; m = m->next, i++)
				if (
					i >= n
					||
					unique[i].x_org != m->screen_geom.position.x
					||
					unique[i].y_org != m->screen_geom.position.y
					||
					unique[i].width != m->screen_geom.sizes.w
					||
					unique[i].height != m->screen_geom.sizes.h
				) {
					dirty = 1;
					m->num = i;

					m->screen_geom = m->window_area_geom =
						basic_geom_create_from_args(
							unique[i].x_org,
							unique[i].y_org,
							unique[i].width,
							unique[i].height
						);
				}
		} else { /* less monitors available nn < n */
			for (i = nn; i < n; i++) {
				for (m = mons; m && m->next; m = m->next);
				while ((c = m->clients)) {
					dirty = 1;
					m->clients = c->next;
					detachstack(c);
					c->mon = mons;
					attach(c);
					attachstack(c);
				}
				if (m == selmon)
					selmon = mons;
				monitor_destroy(m);
			}
		}
		free(unique);
	} else
#endif /* ENABLE_XINERAMA */
	{ /* default monitor setup */
		if (!mons)
			mons = monitor_create();

		if (
			mons->screen_geom.sizes.w != xbase->screen_sizes.w
			||
			mons->screen_geom.sizes.h != xbase->screen_sizes.h
		) {
			dirty = 1;
			mons->screen_geom.sizes =
				mons->window_area_geom.sizes =
				xbase->screen_sizes;
		}
	}
	if (dirty) {
		selmon = mons;
		selmon = wintomon(xbase->x_root);
	}
	return dirty;
}

void updatenumlockmask()
{
	unsigned int i;
	XModifierKeymap *modmap;

	numlockmask = 0;
	modmap = XGetModifierMapping(xbase->x_display);
	for (i = 0; i < 8; i++) {
		for (int j = 0; j < modmap->max_keypermod; j++) {
			if (
				modmap->modifiermap[i * modmap->max_keypermod + j]
				==
				XKeysymToKeycode(xbase->x_display, XK_Num_Lock)
			) {
				numlockmask = (1 << i);
			}
		}
	}
	XFreeModifiermap(modmap);
}

void updatesizehints(Client *c)
{
	XSizeHints size = { 0 };
	long msize = 0;

	if (!XGetWMNormalHints(xbase->x_display, c->x_window, &size, &msize)) {
		/* size is uninitialized, ensure that size.flags aren't used */
		size.flags = PSize;
	}

	client_size_hints_update(&c->size_hints, &size);

	c->state.is_fixed = (
		c->size_hints.maxw
		&&
		c->size_hints.maxh
		&&
		c->size_hints.maxw == c->size_hints.minw
		&&
		c->size_hints.maxh == c->size_hints.minh
	);
}

void updatetitle(Client *c)
{
	if (!gettextprop(c->x_window,
						xbase->atoms->netatom[NetWMName],
						c->state.name,
						sizeof(c->state.name)))
	{
		gettextprop(c->x_window, XA_WM_NAME, c->state.name, sizeof(c->state.name));
	}

	// hack to mark broken clients
	if (c->state.name[0] == '\0') {
		strcpy(c->state.name, broken);
	}
}

void updatewindowtype(Client *c)
{
	Atom state = getatomprop(c, xbase->atoms->netatom[NetWMState]);
	Atom wtype = getatomprop(c, xbase->atoms->netatom[NetWMWindowType]);

	if (state == xbase->atoms->netatom[NetWMFullscreen])
		setfullscreen(c, 1);
	if (wtype == xbase->atoms->netatom[NetWMWindowTypeDialog])
		c->state.is_floating = true;
}

void updatewmhints(Client *c)
{
	XWMHints *wmh = XGetWMHints(xbase->x_display, c->x_window);
	if (!wmh) return;

	if (c == selmon->sel && wmh->flags & XUrgencyHint) {
		wmh->flags &= ~XUrgencyHint;
		XSetWMHints(xbase->x_display, c->x_window, wmh);
	} else {
		c->state.is_urgent = (wmh->flags & XUrgencyHint) ? true : false;
	}

	if (wmh->flags & InputHint) {
		c->state.never_focus = !wmh->input;
	} else {
		c->state.never_focus = false;
	}

	XFree(wmh);
}

Client *wintoclient(Window w)
{
	for (Monitor *m = mons; m; m = m->next) {
		for (Client *c = m->clients; c; c = c->next) {
			if (c->x_window == w) {
				return c;
			}
		}
	}

	return NULL;
}

Monitor *wintomon(Window w)
{
	{
		int x = 0, y = 0;
		if (w == xbase->x_root && getrootptr(&x, &y)) {
			return recttomon(x, y, 1, 1);
		}
	}

	{
		Client *c = wintoclient(w);
		if (c) return c->mon;
	}

	return selmon;
}
