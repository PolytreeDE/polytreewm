#include "dwm.h"

#include "atoms.h"
#include "helpers.h"
#include "layouts.h"
#include "main.h"
#include "settings.h"
#include "spawn.h"
#include "state.h"
#include "unit.h"
#include "util.h"

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

#define INTERSECT(x,y,w,h,basic_geometry) ( \
	MAX(                                    \
		0,                                  \
		MIN(                                \
			(x) + (w),                      \
			basic_geometry.position.x       \
			+                               \
			basic_geometry.sizes.w          \
		)                                   \
		-                                   \
		MAX(                                \
			(x),                            \
			basic_geometry.position.x       \
		)                                   \
	)                                       \
	*                                       \
	MAX(                                    \
		0,                                  \
		MIN(                                \
			(y) + (h),                      \
			basic_geometry.position.y       \
			+                               \
			basic_geometry.sizes.h          \
		)                                   \
		-                                   \
		MAX(                                \
			(y),                            \
			basic_geometry.position.y       \
		)                                   \
	)                                       \
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
	LayoutsSymbolFunc symbol_func;
	void (*arrange)(Monitor *);
} Layout;

struct Monitor {
	struct BasicGeometry screen_geometry;
	struct BasicGeometry window_area_geometry;
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

struct Screen {
	struct Sizes sizes;
	int x_screen;
};

/*************************
 * function declarations *
 *************************/

static int applysizehints(Client *c, ClientGeometry client_geometry, int interact);
static void arrange(Monitor *m);
static void arrangemon(Monitor *m);
static void attach(Client *c);
static void attachstack(Client *c);
static void configborder(const Arg *arg);
static void configgap(const Arg *arg);
static void checkotherwm();
static void cleanup();
static void configure(Client *c);
static void detach(Client *c);
static void detachstack(Client *c);
static Monitor *dirtomon(int dir);
static void dorestart(const Arg *arg);
static void focus(Client *c);
static void focusmon(const Arg *arg);
static void focusstack(const Arg *arg);
static Atom getatomprop(Client *c, Atom prop);
static int getrootptr(int *x, int *y);
static long getstate(Window w);
static int gettextprop(Window w, Atom atom, char *text, unsigned int size);
static void grabbuttons(Client *c, int focused);
static void grabkeys();
static void incnmaster(const Arg *arg);
static int isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info);
static void killclient(const Arg *arg);
static void manage(Window w, XWindowAttributes *wa);
static Monitor *monitor_create();
static void monitor_destroy(Monitor *mon);
static void movemouse(const Arg *arg);
static void movestack(const Arg *arg);
static Client *nexttiled(Client *c);
static void pop(Client *);
static void quit(const Arg *arg);
static Monitor *recttomon(int x, int y, int w, int h);
static void resetnmaster(const Arg *arg);
static void resize(Client *c, struct ClientGeometry client_geometry, int interact);
static void resizeclient(Client *c, struct ClientGeometry client_geometry);
static void resizemouse(const Arg *arg);
static void restack(Monitor *m);
static void run();
static void scan();
static void scheme_destroy();
static int sendevent(Client *c, Atom proto);
static void sendmon(Client *c, Monitor *m);
static void setclientstate(Client *c, long state);
static void setfocus(Client *c);
static void setfullscreen(Client *c, int fullscreen);
static void setlayout(const Arg *arg);
static void setmfact(const Arg *arg);
static bool setup();
static void seturgent(Client *c, bool is_urgent);
static void showhide(Client *c);
static void spawn(const Arg *arg);
static void spawn_callback();
static void tagmon(const Arg *arg);
static void togglefloating(const Arg *arg);
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
static void wmcheckwin_create();
static void wmcheckwin_destroy();
static void zoom(const Arg *arg);

#include "dwm/handlers.h"
#include "dwm/layouts.h"
#include "dwm/xerror.h"

/*************
 * variables *
 *************/

static const char *program_title = NULL;

static struct Screen screen = {
	.sizes = { 0, 0 },
	.x_screen = 0,
};

static Unit global_unit = NULL;

static const char broken[] = "broken";
static int (*xerrorxlib)(Display *, XErrorEvent *);
static unsigned int numlockmask = 0;
static Atoms atoms = NULL;
static int running = 1;
static Cur *cursor[CurLast];
static Clr **scheme;
static Display *dpy;
static Drw *drw;
static Monitor *mons, *selmon;
static Window root, wmcheckwin;

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

#include "config.h"

/************************************
 * Private function implementations *
 ************************************/

#include "dwm/handlers.c"
#include "dwm/layouts.c"
#include "dwm/xerror.c"

/***********************************
 * Public function implementations *
 ***********************************/

int dwm_main(const char *const new_program_title)
{
	if (!(program_title = new_program_title)) {
		fatal("no program title is given");
	}

	if (!XSupportsLocale()) {
		warning("no locale support in X");
	}

	// Resource allocations start here.

	if (!(dpy = XOpenDisplay(NULL))) {
		fatal("cannot open display");
	}

	checkotherwm();

	if (!(atoms = atoms_create(dpy))) {
		fatal("cannot create atoms");
	}

	if (!(global_unit = unit_new(UNIT_GLOBAL, NULL))) {
		fatal("cannot create atoms");
	}

	// Old code.

	if (!setup()) {
		fatal("cannot setup");
	}

	scan();
	run();

	cleanup();

	// Resource cleanups.

	UNIT_DELETE(global_unit);
	ATOMS_DELETE(atoms);
	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}

/************************************
 * Private function implementations *
 ************************************/

int applysizehints(
	Client *c,
	ClientGeometry client_geometry,
	int interact
) {
	int *const x = &client_geometry->basic.position.x;
	int *const y = &client_geometry->basic.position.y;
	int *const w = &client_geometry->basic.sizes.w;
	int *const h = &client_geometry->basic.sizes.h;
	int const bw = client_geometry->border_width;

	Monitor *m = c->mon;

	/* set minimum possible */
	*w = MAX(1, *w);
	*h = MAX(1, *h);

	if (interact) {
		if (*x > screen.sizes.w) {
			*x =
				screen.sizes.w
				-
				client_geometry_total_width(&c->state.geometry);
		}
		if (*y > screen.sizes.h) {
			*y =
				screen.sizes.h
				-
				client_geometry_total_height(&c->state.geometry);
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
			m->window_area_geometry.position.x + m->window_area_geometry.sizes.w
		) {
			*x =
				m->window_area_geometry.position.x
				+
				m->window_area_geometry.sizes.w
				-
				client_geometry_total_width(&c->state.geometry);
		}
		if (
			*y
			>=
			m->window_area_geometry.position.y + m->window_area_geometry.sizes.h
		) {
			*y =
				m->window_area_geometry.position.y
				+
				m->window_area_geometry.sizes.h
				-
				client_geometry_total_height(&c->state.geometry);
		}
		if (*x + *w + 2 * bw <= m->window_area_geometry.position.x) {
			*x = m->window_area_geometry.position.x;
		}
		if (*y + *h + 2 * bw <= m->window_area_geometry.position.y) {
			*y = m->window_area_geometry.position.y;
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
		*x != c->state.geometry.basic.position.x
		||
		*y != c->state.geometry.basic.position.y
		||
		*w != c->state.geometry.basic.sizes.w
		||
		*h != c->state.geometry.basic.sizes.h
		||
		bw != c->state.geometry.border_width
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

void checkotherwm()
{
	xerrorxlib = XSetErrorHandler(xerrorstart);
	/* this causes an error if some other window manager is running */
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
	XSync(dpy, False);
	XSetErrorHandler(xerror);
	XSync(dpy, False);
}

void cleanup()
{
	Layout foo = { NULL, NULL };
	selmon->lt[selmon->sellt] = &foo;

	for (Monitor *m = mons; m; m = m->next) {
		while (m->stack) {
			unmanage(m->stack, 0);
		}
	}

	XUngrabKey(dpy, AnyKey, AnyModifier, root);

	while (mons) {
		monitor_destroy(mons);
	}

	for (size_t i = 0; i < CurLast; i++) {
		drw_cur_free(drw, cursor[i]);
	}

	scheme_destroy();

	wmcheckwin_destroy();
	drw_free(drw);
	XSync(dpy, False);
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XDeleteProperty(dpy, root, atoms->netatom[NetActiveWindow]);
}

void configure(Client *c)
{
	XConfigureEvent ce = {
		.type = ConfigureNotify,
		.serial = 0,
		.send_event = False,
		.display = dpy,
		.event = c->x_window,
		.window = c->x_window,
		.x = c->state.geometry.basic.position.x,
		.y = c->state.geometry.basic.position.y,
		.width = c->state.geometry.basic.sizes.w,
		.height = c->state.geometry.basic.sizes.h,
		.border_width = c->state.geometry.border_width,
		.above = None,
		.override_redirect = False,
	};

	XSendEvent(dpy, c->x_window, False, StructureNotifyMask, (XEvent*)&ce);
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

void dorestart(__attribute__((unused)) const Arg *const arg)
{
	restart();
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
		XSetWindowBorder(dpy, c->x_window, scheme[SchemeSel][ColBorder].pixel);
		setfocus(c);
	} else {
		XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
		XDeleteProperty(dpy, root, atoms->netatom[NetActiveWindow]);
	}
	selmon->sel = c;
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

Atom getatomprop(Client *c, Atom prop)
{
	Atom atom = None;

	Atom da = None;
	int di = 0;
	unsigned long dl = 0;
	unsigned char *p = NULL;

	if (
		XGetWindowProperty(
			dpy,
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

	return XQueryPointer(dpy, root, &dummy, &dummy, x, y, &di, &di, &dui);
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
			dpy,
			w,
			atoms->wmatom[WMState],
			0L,
			2L,
			False,
			atoms->wmatom[WMState],
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

	if (!XGetTextProperty(dpy, w, &name, atom) || !name.nitems) {
		return 0;
	}

	if (name.encoding == XA_STRING) {
		strncpy(text, (char *)name.value, size - 1);
	} else {
		if (
			XmbTextPropertyToTextList(dpy, &name, &list, &n) >= Success
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
		XUngrabButton(dpy, AnyButton, AnyModifier, c->x_window);

		if (!focused) {
			XGrabButton(
				dpy,
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
						dpy,
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

		XUngrabKey(dpy, AnyKey, AnyModifier, root);

		for (i = 0; i < LENGTH(keys); i++) {
			if ((code = XKeysymToKeycode(dpy, keys[i].keysym))) {
				for (j = 0; j < LENGTH(modifiers); j++) {
					XGrabKey(
						dpy,
						code,
						keys[i].mod | modifiers[j],
						root,
						True,
						GrabModeAsync,
						GrabModeAsync
					);
				}
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

void killclient(__attribute__((unused)) const Arg *arg)
{
	if (!selmon->sel)
		return;
	if (!sendevent(selmon->sel, atoms->wmatom[WMDelete])) {
		XGrabServer(dpy);
		XSetErrorHandler(xerrordummy);
		XSetCloseDownMode(dpy, DestroyAll);
		XKillClient(dpy, selmon->sel->x_window);
		XSync(dpy, False);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
}

void manage(Window w, XWindowAttributes *wa)
{
	Client *const c = ecalloc(1, sizeof(Client));

	client_state_init(&c->state);

	c->x_window = w;
	c->state.geometry.basic.position.x = wa->x;
	c->state.geometry.basic.position.y = wa->y;
	c->state.geometry.basic.sizes.w = wa->width;
	c->state.geometry.basic.sizes.h = wa->height;
	c->state.is_floating = false;

	updatetitle(c);

	Window trans = None;

	{
		Client *t = NULL;

		if (XGetTransientForHint(dpy, w, &trans) && (t = wintoclient(trans))) {
			c->mon = t->mon;
		} else {
			c->mon = selmon;
		}
	}

	client_geometry_adjust_to_boundary(
		&c->state.geometry,
		&c->mon->screen_geometry
	);

	c->state.geometry.border_width = settings_get_border_width();

	{
		XWindowChanges wc;
		wc.border_width = c->state.geometry.border_width;
		XConfigureWindow(dpy, w, CWBorderWidth, &wc);
	}

	XSetWindowBorder(dpy, w, scheme[SchemeNorm][ColBorder].pixel);

	configure(c); /* propagates border_width, if size doesn't change */
	updatewindowtype(c);
	updatesizehints(c);
	updatewmhints(c);

	{
		const int total_width = client_geometry_total_width(&c->state.geometry);
		const int total_height =
			client_geometry_total_height(&c->state.geometry);

		c->state.geometry.basic.position.x =
			c->mon->screen_geometry.position.x
			+
			(c->mon->screen_geometry.sizes.w - total_width) / 2;
		c->state.geometry.basic.position.y =
			c->mon->screen_geometry.position.y
			+
			(c->mon->screen_geometry.sizes.h - total_height) / 2;
	}

	XSelectInput(
		dpy,
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
		XRaiseWindow(dpy, c->x_window);
	}

	attach(c);
	attachstack(c);

	XChangeProperty(
		dpy,
		root,
		atoms->netatom[NetClientList],
		XA_WINDOW,
		32,
		PropModeAppend,
		(unsigned char*)&(c->x_window),
		1
	);

	/* some windows require this */
	XMoveResizeWindow(
		dpy,
		c->x_window,
		c->state.geometry.basic.position.x + 2 * screen.sizes.w,
		c->state.geometry.basic.position.y,
		c->state.geometry.basic.sizes.w,
		c->state.geometry.basic.sizes.h
	);

	setclientstate(c, NormalState);

	if (c->mon == selmon) {
		unfocus(selmon->sel, 0);
	}

	c->mon->sel = c;

	arrange(c->mon);
	XMapWindow(dpy, c->x_window);
	focus(NULL);
}

Monitor *monitor_create()
{
	Monitor *const m = ecalloc(1, sizeof(Monitor));

	m->screen_geometry      = basic_geometry_create();
	m->window_area_geometry = basic_geometry_create();

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

void movemouse(__attribute__((unused)) const Arg *arg)
{
	Client *const c = selmon->sel;
	if (c == NULL) return;

	restack(selmon);

	if (
		XGrabPointer(
			dpy,
			root,
			False,
			MOUSEMASK,
			GrabModeAsync,
			GrabModeAsync,
			None,
			cursor[CurMove]->cursor,
			CurrentTime
		) != GrabSuccess
	) {
		return;
	}

	int x, y;
	if (!getrootptr(&x, &y)) return;

	const unsigned int snap_distance = settings_get_snap_distance();
	const int ocx = c->state.geometry.basic.position.x;
	const int ocy = c->state.geometry.basic.position.y;

	Time lasttime = 0;

	XEvent ev;

	do {
		XMaskEvent(
			dpy,
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
				abs(selmon->window_area_geometry.position.x - nx)
				<
				snap_distance
			) {
				nx = selmon->window_area_geometry.position.x;
			} else if (
				abs(
					(
						selmon->window_area_geometry.position.x
						+
						selmon->window_area_geometry.sizes.w
					)
					-
					(nx + client_geometry_total_width(&c->state.geometry))
				)
				<
				snap_distance
			) {
				nx =
					selmon->window_area_geometry.position.x
					+
					selmon->window_area_geometry.sizes.w
					-
					client_geometry_total_width(&c->state.geometry);
			}

			if (
				abs(selmon->window_area_geometry.position.y - ny)
				<
				snap_distance
			) {
				ny = selmon->window_area_geometry.position.y;
			} else if (
				abs(
					(
						selmon->window_area_geometry.position.y
						+
						selmon->window_area_geometry.sizes.h
					) - (ny + client_geometry_total_height(&c->state.geometry))
				)
				<
				snap_distance
			) {
				ny =
					selmon->window_area_geometry.position.y
					+
					selmon->window_area_geometry.sizes.h
					-
					client_geometry_total_height(&c->state.geometry);
			}

			if (
				!c->state.is_floating
				&&
				(
					abs(nx - c->state.geometry.basic.position.x) > snap_distance
					||
					abs(ny - c->state.geometry.basic.position.y) > snap_distance)
			) {
				togglefloating(NULL);
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->state.is_floating) {
				struct ClientGeometry client_geometry = c->state.geometry;
				position_init_from_args(&client_geometry.basic.position, nx, ny);
				resize(c, client_geometry, 1);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XUngrabPointer(dpy, CurrentTime);

	Monitor *const m = recttomon(
		c->state.geometry.basic.position.x,
		c->state.geometry.basic.position.y,
		c->state.geometry.basic.sizes.w,
		c->state.geometry.basic.sizes.h
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

void quit(__attribute__((unused)) const Arg *arg)
{
	running = 0;
}

Monitor *recttomon(int x, int y, int w, int h)
{
	Monitor *m, *r = selmon;
	int a, area = 0;

	for (m = mons; m; m = m->next)
		if ((a = INTERSECT(x, y, w, h, m->window_area_geometry)) > area) {
			area = a;
			r = m;
		}
	return r;
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

void resize(Client *c, struct ClientGeometry client_geometry, int interact)
{
	if (applysizehints(c, &client_geometry, interact)) {
		resizeclient(c, client_geometry);
	}
}

void resizeclient(Client *c, const struct ClientGeometry client_geometry)
{
	c->state.geometry = client_geometry;

	XWindowChanges wc = { 0 };
	client_geometry_convert_to_x_window_changes(&client_geometry, &wc);

	XConfigureWindow(
		dpy,
		c->x_window,
		CWX | CWY | CWWidth | CWHeight | CWBorderWidth,
		&wc
	);

	configure(c);
	XSync(dpy, False);
}

void resizemouse(__attribute__((unused)) const Arg *arg)
{
	Client *const c = selmon->sel;
	if (c == NULL) return;

	restack(selmon);

	if (
		XGrabPointer(
			dpy,
			root,
			False,
			MOUSEMASK,
			GrabModeAsync,
			GrabModeAsync,
			None,
			cursor[CurResize]->cursor,
			CurrentTime
		) != GrabSuccess
	) {
		return;
	}

	XWarpPointer(
		dpy,
		None,
		c->x_window,
		0,
		0,
		0,
		0,
		c->state.geometry.basic.sizes.w + c->state.geometry.border_width - 1,
		c->state.geometry.basic.sizes.h + c->state.geometry.border_width - 1
	);

	const unsigned int snap_distance = settings_get_snap_distance();
	const int ocx = c->state.geometry.basic.position.x;
	const int ocy = c->state.geometry.basic.position.y;

	Time lasttime = 0;

	XEvent ev;

	do {
		XMaskEvent(
			dpy,
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
				ev.xmotion.x - ocx - 2 * c->state.geometry.border_width + 1,
				1
			);
			const int nh = MAX(
				ev.xmotion.y - ocy - 2 * c->state.geometry.border_width + 1,
				1
			);

			if (
				(
					c->mon->window_area_geometry.position.x + nw
					>=
					selmon->window_area_geometry.position.x
				)
				&&
				(
					c->mon->window_area_geometry.position.x + nw
					<=
					(
						selmon->window_area_geometry.position.x
						+
						selmon->window_area_geometry.sizes.w
					)
				)
				&&
				(
					c->mon->window_area_geometry.position.y + nh
					>=
					selmon->window_area_geometry.position.y
				)
				&&
				(
					c->mon->window_area_geometry.position.y + nh
					<=
					(
						selmon->window_area_geometry.position.y
						+
						selmon->window_area_geometry.sizes.h
					)
				)
			) {
				if (
					!c->state.is_floating
					&&
					(
						selmon->lt[selmon->sellt]->arrange == NULL
						||
						abs(nw - c->state.geometry.basic.sizes.w) > snap_distance
						||
						abs(nh - c->state.geometry.basic.sizes.h) > snap_distance
					)
				) {
					togglefloating(NULL);
				}
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->state.is_floating) {
				struct ClientGeometry client_geometry = c->state.geometry;
				sizes_init_from_args(&client_geometry.basic.sizes, nw, nh);
				resize(c, client_geometry, 1);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XWarpPointer(
		dpy,
		None,
		c->x_window,
		0,
		0,
		0,
		0,
		c->state.geometry.basic.sizes.w + c->state.geometry.border_width - 1,
		c->state.geometry.basic.sizes.h + c->state.geometry.border_width - 1
	);

	XUngrabPointer(dpy, CurrentTime);

	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));

	Monitor *const m = recttomon(
		c->state.geometry.basic.position.x,
		c->state.geometry.basic.position.y,
		c->state.geometry.basic.sizes.w,
		c->state.geometry.basic.sizes.h
	);

	if (m != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
}

void restack(Monitor *m)
{
	Client *c;
	XEvent ev;
	XWindowChanges wc;

	if (!m->sel) return;

	if (m->sel->state.is_floating || !m->lt[m->sellt]->arrange) {
		XRaiseWindow(dpy, m->sel->x_window);
	}

	if (m->lt[m->sellt]->arrange) {
		wc.stack_mode = Below;
		// TODO: Learn what is sibling and what
		// is the following line responsible for.
		// wc.sibling = m->bar->barwin;
		for (c = m->stack; c; c = c->snext) {
			if (!c->state.is_floating && ISVISIBLE(c)) {
				XConfigureWindow(
					dpy,
					c->x_window,
					CWSibling | CWStackMode,
					&wc
				);
				wc.sibling = c->x_window;
			}
		}
	}
	XSync(dpy, False);
	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}

void run()
{
	XEvent ev;
	/* main event loop */
	XSync(dpy, False);
	while (running && !XNextEvent(dpy, &ev)) {
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

	if (XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
		for (i = 0; i < num; i++) {
			if (
				!XGetWindowAttributes(dpy, wins[i], &wa)
				||
				wa.override_redirect
				||
				XGetTransientForHint(dpy, wins[i], &d1)
			) {
				continue;
			}

			if (
				wa.map_state == IsViewable
				||
				getstate(wins[i]) == IconicState
			) {
				manage(wins[i], &wa);
			}
		}
		for (i = 0; i < num; i++) { /* now the transients */
			if (!XGetWindowAttributes(dpy, wins[i], &wa)) {
				continue;
			}
			if (
				XGetTransientForHint(dpy, wins[i], &d1)
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
		dpy,
		c->x_window,
		atoms->wmatom[WMState],
		atoms->wmatom[WMState],
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

	if (XGetWMProtocols(dpy, c->x_window, &protocols, &n)) {
		while (!exists && n--) {
			exists = protocols[n] == proto;
		}
		XFree(protocols);
	}

	if (exists) {
		ev.type = ClientMessage;
		ev.xclient.window = c->x_window;
		ev.xclient.message_type = atoms->wmatom[WMProtocols];
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = proto;
		ev.xclient.data.l[1] = CurrentTime;
		XSendEvent(dpy, c->x_window, False, NoEventMask, &ev);
	}

	return exists;
}

void setfocus(Client *c)
{
	if (!c->state.never_focus) {
		XSetInputFocus(dpy, c->x_window, RevertToPointerRoot, CurrentTime);
		XChangeProperty(
			dpy,
			root,
			atoms->netatom[NetActiveWindow],
			XA_WINDOW,
			32,
			PropModeReplace,
			(unsigned char*)&(c->x_window),
			1
		);
	}

	sendevent(c, atoms->wmatom[WMTakeFocus]);
}

void setfullscreen(Client *c, int fullscreen)
{
	if (fullscreen && !c->state.is_fullscreen) {
		XChangeProperty(
			dpy,
			c->x_window,
			atoms->netatom[NetWMState],
			XA_ATOM,
			32,
			PropModeReplace,
			(unsigned char*)&atoms->netatom[NetWMFullscreen],
			1
		);
		c->state.is_fullscreen = true;
		// We have to rearrange because borders and gaps may have changed.
		arrange(c->mon);
	} else if (!fullscreen && c->state.is_fullscreen){
		XChangeProperty(
			dpy,
			c->x_window,
			atoms->netatom[NetWMState],
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

bool setup()
{
	XSetWindowAttributes wa;

	/* init screen */
	screen.x_screen = DefaultScreen(dpy);
	screen.sizes.w = DisplayWidth(dpy, screen.x_screen);
	screen.sizes.h = DisplayHeight(dpy, screen.x_screen);
	root = RootWindow(dpy, screen.x_screen);
	drw = drw_create(dpy, screen.x_screen, root, screen.sizes.w, screen.sizes.h);
	if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
		fatal("no fonts could be loaded.");
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
		dpy,
		root,
		atoms->netatom[NetSupported],
		XA_ATOM,
		32,
		PropModeReplace,
		(unsigned char*)atoms->netatom,
		NetLast
	);
	XDeleteProperty(dpy, root, atoms->netatom[NetClientList]);

	/* select events */
	wa.cursor = cursor[CurNormal]->cursor;
	wa.event_mask =
		SubstructureRedirectMask | SubstructureNotifyMask | ButtonPressMask |
		PointerMotionMask | EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask | PropertyChangeMask;
	XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);
	XSelectInput(dpy, root, wa.event_mask);
	grabkeys();
	focus(NULL);

	return true;
}

void seturgent(Client *c, bool is_urgent)
{
	XWMHints *wmh;

	c->state.is_urgent = is_urgent;
	if (!(wmh = XGetWMHints(dpy, c->x_window))) return;

	wmh->flags =
		is_urgent ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
	XSetWMHints(dpy, c->x_window, wmh);
	XFree(wmh);
}

void showhide(Client *c)
{
	if (!c)
		return;
	if (ISVISIBLE(c)) {
		/* show clients top down */
		XMoveWindow(
			dpy,
			c->x_window,
			c->state.geometry.basic.position.x,
			c->state.geometry.basic.position.y
		);

		if (!c->mon->lt[c->mon->sellt]->arrange || c->state.is_floating) {
			resize(c, c->state.geometry, 0);
		}
		showhide(c->snext);
	} else {
		/* hide clients bottom up */
		showhide(c->snext);
		XMoveWindow(
			dpy,
			c->x_window,
			client_geometry_total_width(&c->state.geometry) * -2,
			c->state.geometry.basic.position.y
		);
	}
}

void spawn(const Arg *arg)
{
	const char *const command_name = arg->v;

	spawn_command(command_name, spawn_callback, selmon->num);
}

void spawn_callback()
{
	if (dpy) {
		close(ConnectionNumber(dpy));
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
		struct ClientGeometry client_geometry = selmon->sel->state.geometry;

		client_geometry.basic.sizes = sizes_create_from_args(
			selmon->sel->state.geometry.basic.sizes.w -
				2 * (border_width - selmon->sel->state.geometry.border_width),
			selmon->sel->state.geometry.basic.sizes.h -
				2 * (border_width - selmon->sel->state.geometry.border_width)
		);

		client_geometry.border_width = border_width;

		resize(selmon->sel, client_geometry, 0);
	}

	arrange(selmon);
}

void unfocus(Client *c, int setfocus)
{
	if (!c)
		return;
	grabbuttons(c, 0);
	XSetWindowBorder(dpy, c->x_window, scheme[SchemeNorm][ColBorder].pixel);
	if (setfocus) {
		XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
		XDeleteProperty(dpy, root, atoms->netatom[NetActiveWindow]);
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
		XGrabServer(dpy); /* avoid race conditions */
		XSetErrorHandler(xerrordummy);
		XConfigureWindow(dpy, c->x_window, CWBorderWidth, &wc); /* remove border */
		XUngrabButton(dpy, AnyButton, AnyModifier, c->x_window);
		setclientstate(c, WithdrawnState);
		XSync(dpy, False);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
	free(c);
	focus(NULL);
	updateclientlist();
	arrange(m);
}

void updateclientlist()
{
	XDeleteProperty(dpy, root, atoms->netatom[NetClientList]);

	for (Monitor *m = mons; m; m = m->next) {
		for (Client *c = m->clients; c; c = c->next) {
			XChangeProperty(
				dpy,
				root,
				atoms->netatom[NetClientList],
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
	if (XineramaIsActive(dpy)) {
		int i, j, n, nn;
		Client *c;
		Monitor *m;
		XineramaScreenInfo *info = XineramaQueryScreens(dpy, &nn);
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
					unique[i].x_org != m->screen_geometry.position.x
					||
					unique[i].y_org != m->screen_geometry.position.y
					||
					unique[i].width != m->screen_geometry.sizes.w
					||
					unique[i].height != m->screen_geometry.sizes.h
				) {
					dirty = 1;
					m->num = i;

					m->screen_geometry = m->window_area_geometry =
						basic_geometry_create_from_args(
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
			mons->screen_geometry.sizes.w != screen.sizes.w
			||
			mons->screen_geometry.sizes.h != screen.sizes.h
		) {
			dirty = 1;
			mons->screen_geometry.sizes =
				mons->window_area_geometry.sizes =
				screen.sizes;
		}
	}
	if (dirty) {
		selmon = mons;
		selmon = wintomon(root);
	}
	return dirty;
}

void updatenumlockmask()
{
	unsigned int i;
	XModifierKeymap *modmap;

	numlockmask = 0;
	modmap = XGetModifierMapping(dpy);
	for (i = 0; i < 8; i++) {
		for (int j = 0; j < modmap->max_keypermod; j++) {
			if (
				modmap->modifiermap[i * modmap->max_keypermod + j]
				==
				XKeysymToKeycode(dpy, XK_Num_Lock)
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

	if (!XGetWMNormalHints(dpy, c->x_window, &size, &msize)) {
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
	if (!gettextprop(c->x_window, atoms->netatom[NetWMName], c->state.name, sizeof(c->state.name)))
		gettextprop(c->x_window, XA_WM_NAME, c->state.name, sizeof(c->state.name));
	if (c->state.name[0] == '\0') /* hack to mark broken clients */
		strcpy(c->state.name, broken);
}

void updatewindowtype(Client *c)
{
	Atom state = getatomprop(c, atoms->netatom[NetWMState]);
	Atom wtype = getatomprop(c, atoms->netatom[NetWMWindowType]);

	if (state == atoms->netatom[NetWMFullscreen])
		setfullscreen(c, 1);
	if (wtype == atoms->netatom[NetWMWindowTypeDialog])
		c->state.is_floating = true;
}

void updatewmhints(Client *c)
{
	XWMHints *wmh = XGetWMHints(dpy, c->x_window);
	if (!wmh) return;

	if (c == selmon->sel && wmh->flags & XUrgencyHint) {
		wmh->flags &= ~XUrgencyHint;
		XSetWMHints(dpy, c->x_window, wmh);
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
		if (w == root && getrootptr(&x, &y)) {
			return recttomon(x, y, 1, 1);
		}
	}

	{
		Client *c = wintoclient(w);
		if (c) return c->mon;
	}

	return selmon;
}

void wmcheckwin_create()
{
	wmcheckwin = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);

	XChangeProperty(
		dpy,
		wmcheckwin,
		atoms->netatom[NetWMCheck],
		XA_WINDOW,
		32,
		PropModeReplace,
		(unsigned char*)&wmcheckwin,
		1
	);
	XChangeProperty(
		dpy,
		wmcheckwin,
		atoms->netatom[NetWMName],
		atoms->utf8string,
		8,
		PropModeReplace,
		(unsigned char*)
		program_title,
		strlen(program_title)
	);
	XChangeProperty(
		dpy,
		root,
		atoms->netatom[NetWMCheck],
		XA_WINDOW,
		32,
		PropModeReplace,
		(unsigned char*)&wmcheckwin,
		1
	);
}

void wmcheckwin_destroy()
{
	XDestroyWindow(dpy, wmcheckwin);
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
