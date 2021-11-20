#include "atoms.h"
#include "helpers.h"
#include "layouts.h"
#include "settings.h"
#include "spawn.h"
#include "state.h"
#include "unit.h"
#include "util.h"

#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
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
#define WIDTH(X)     ((X)->state.geometry.basic.w + 2 * (X)->state.geometry.bw)
#define HEIGHT(X)    ((X)->state.geometry.basic.h + 2 * (X)->state.geometry.bw)

#define CLEANMASK(mask) (                            \
	(mask) &                                         \
	~(numlockmask | LockMask) &                      \
	(ShiftMask | ControlMask | Mod1Mask | Mod2Mask | \
		Mod3Mask | Mod4Mask | Mod5Mask)              \
)

#define INTERSECT(x,y,w,h,m) (                                                    \
	MAX(                                                                          \
		0,                                                                        \
		MIN((x) + (w), (m)->window_area_geometry.x + (m)->window_area_geometry.w) \
		-                                                                         \
		MAX((x), (m)->window_area_geometry.x)                                     \
	)                                                                             \
	*                                                                             \
	MAX(                                                                          \
		0,                                                                        \
		MIN((y) + (h), (m)->window_area_geometry.y + (m)->window_area_geometry.h) \
		-                                                                         \
		MAX((y), (m)->window_area_geometry.y)                                     \
	)                                                                             \
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
	Window win;
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

/*************************
 * function declarations *
 *************************/

static int applysizehints(Client *c, int *x, int *y, int *w, int *h, int bw, int interact);
static void arrange(Monitor *m);
static void arrangemon(Monitor *m);
static void attach(Client *c);
static void attachstack(Client *c);
static void configborder(const Arg *arg);
static void configgap(const Arg *arg);
static void checkotherwm();
static void cleanup();
static void cleanupmon(Monitor *mon);
static void configure(Client *c);
static Monitor *createmon();
static void detach(Client *c);
static void detachstack(Client *c);
static Monitor *dirtomon(int dir);
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
static void movemouse(const Arg *arg);
static void movestack(const Arg *arg);
static Client *nexttiled(Client *c);
static void pop(Client *);
static void quit(const Arg *arg);
static Monitor *recttomon(int x, int y, int w, int h);
static void resetnmaster(const Arg *arg);
static void resize(Client *c, int x, int y, int w, int h, int bw, int interact);
static void resizeclient(Client *c, int x, int y, int w, int h, int bw);
static void resizemouse(const Arg *arg);
static void restack(Monitor *m);
static void run();
static void scan();
static int sendevent(Client *c, Atom proto);
static void sendmon(Client *c, Monitor *m);
static void setclientstate(Client *c, long state);
static void setfocus(Client *c);
static void setfullscreen(Client *c, int fullscreen);
static void setlayout(const Arg *arg);
static void setmfact(const Arg *arg);
static bool setup();
static void seturgent(Client *c, int urg);
static void showhide(Client *c);
static void sigchld(int unused);
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
static void zoom(const Arg *arg);

#include "dwm/handlers.h"
#include "dwm/layouts.h"
#include "dwm/xerror.h"

/*************
 * variables *
 *************/

static Unit global_unit = NULL;
static const char broken[] = "broken";
static int screen;
static int sw, sh;           /* X display screen geometry width, height */
static int (*xerrorxlib)(Display *, XErrorEvent *);
static unsigned int numlockmask = 0;
static void (*handler[LASTEvent]) (XEvent *) = {
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
static Atoms atoms = NULL;
static int running = 1;
static Cur *cursor[CurLast];
static Clr **scheme;
static Display *dpy;
static Drw *drw;
static Monitor *mons, *selmon;
static Window root, wmcheckwin;

/***************************************************************
 * configuration, allows nested code to access above variables *
 ***************************************************************/

#include "config.h"

/****************************
 * function implementations *
 ****************************/

#include "dwm/handlers.c"
#include "dwm/layouts.c"
#include "dwm/xerror.c"

int main(int argc, char *argv[])
{
	if (argc == 2 && !strcmp("-v", argv[1])) {
		die("polytreewm-"VERSION);
	}

	if (argc != 1) {
		die("usage: polytreewm [-v]");
	}

	if (!setlocale(LC_CTYPE, "") || !XSupportsLocale()) {
		fputs("warning: no locale support\n", stderr);
	}

	if (!(dpy = XOpenDisplay(NULL))) {
		die("polytreewm: cannot open display");
	}

	checkotherwm();

	if (!setup()) {
		die("polytreewm: cannot setup");
	}

#ifdef __OpenBSD__
	if (pledge("stdio rpath proc exec", NULL) == -1) {
		die("pledge");
	}
#endif /* __OpenBSD__ */

	scan();
	run();
	cleanup();
	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}

int applysizehints(
	Client *c,
	int *x,
	int *y,
	int *w,
	int *h,
	int bw,
	int interact
) {
	Monitor *m = c->mon;

	/* set minimum possible */
	*w = MAX(1, *w);
	*h = MAX(1, *h);

	if (interact) {
		if (*x > sw) {
			*x = sw - WIDTH(c);
		}
		if (*y > sh) {
			*y = sh - HEIGHT(c);
		}
		if (*x + *w + 2 * bw < 0) {
			*x = 0;
		}
		if (*y + *h + 2 * bw < 0) {
			*y = 0;
		}
	} else {
		if (*x >= m->window_area_geometry.x + m->window_area_geometry.w) {
			*x =
				m->window_area_geometry.x
				+
				m->window_area_geometry.w
				-
				WIDTH(c);
		}
		if (*y >= m->window_area_geometry.y + m->window_area_geometry.h) {
			*y =
				m->window_area_geometry.y
				+
				m->window_area_geometry.h
				-
				HEIGHT(c);
		}
		if (*x + *w + 2 * bw <= m->window_area_geometry.x) {
			*x = m->window_area_geometry.x;
		}
		if (*y + *h + 2 * bw <= m->window_area_geometry.y) {
			*y = m->window_area_geometry.y;
		}
	}

	if (
		c->state.isfloating
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
		*x != c->state.geometry.basic.x
		||
		*y != c->state.geometry.basic.y
		||
		*w != c->state.geometry.basic.w
		||
		*h != c->state.geometry.basic.h
		||
		bw != c->state.geometry.bw
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
		cleanupmon(mons);
	}

	for (size_t i = 0; i < CurLast; i++) {
		drw_cur_free(drw, cursor[i]);
	}

	for (size_t i = 0; i < LENGTH(colors); i++) {
		free(scheme[i]);
	}

	atoms_destroy(atoms);
	XDestroyWindow(dpy, wmcheckwin);
	drw_free(drw);
	XSync(dpy, False);
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XDeleteProperty(dpy, root, atoms->netatom[NetActiveWindow]);

	if (global_unit) UNIT_DELETE(global_unit);
}

void cleanupmon(Monitor *mon)
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

void configure(Client *c)
{
	XConfigureEvent ce = {
		.type = ConfigureNotify,
		.serial = 0,
		.send_event = False,
		.display = dpy,
		.event = c->win,
		.window = c->win,
		.x = c->state.geometry.basic.x,
		.y = c->state.geometry.basic.y,
		.width = c->state.geometry.basic.w,
		.height = c->state.geometry.basic.h,
		.border_width = c->state.geometry.bw,
		.above = None,
		.override_redirect = False,
	};

	XSendEvent(dpy, c->win, False, StructureNotifyMask, (XEvent*)&ce);
}

Monitor *createmon()
{
	Monitor *const m = ecalloc(1, sizeof(Monitor));

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
		if (c->state.isurgent)
			seturgent(c, 0);
		detachstack(c);
		attachstack(c);
		grabbuttons(c, 1);
		XSetWindowBorder(dpy, c->win, scheme[SchemeSel][ColBorder].pixel);
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
			c->win,
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

	if (XGetWindowProperty(dpy, w, atoms->wmatom[WMState], 0L, 2L, False, atoms->wmatom[WMState],
		&real, &format, &n, &extra, (unsigned char **)&p) != Success)
		return -1;
	if (n != 0)
		result = *p;
	XFree(p);
	return result;
}

int gettextprop(Window w, Atom atom, char *text, unsigned int size)
{
	char **list = NULL;
	int n;
	XTextProperty name;

	if (!text || size == 0)
		return 0;
	text[0] = '\0';
	if (!XGetTextProperty(dpy, w, &name, atom) || !name.nitems)
		return 0;
	if (name.encoding == XA_STRING)
		strncpy(text, (char *)name.value, size - 1);
	else {
		if (XmbTextPropertyToTextList(dpy, &name, &list, &n) >= Success && n > 0 && *list) {
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
		XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
		if (!focused)
			XGrabButton(dpy, AnyButton, AnyModifier, c->win, False,
				BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
		for (i = 0; i < LENGTH(buttons); i++)
			if (buttons[i].click == ClkClientWin)
				for (j = 0; j < LENGTH(modifiers); j++)
					XGrabButton(dpy, buttons[i].button,
						buttons[i].mask | modifiers[j],
						c->win, False, BUTTONMASK,
						GrabModeAsync, GrabModeSync, None, None);
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
		for (i = 0; i < LENGTH(keys); i++)
			if ((code = XKeysymToKeycode(dpy, keys[i].keysym)))
				for (j = 0; j < LENGTH(modifiers); j++)
					XGrabKey(dpy, code, keys[i].mod | modifiers[j], root,
						True, GrabModeAsync, GrabModeAsync);
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
		XKillClient(dpy, selmon->sel->win);
		XSync(dpy, False);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
}

void manage(Window w, XWindowAttributes *wa)
{
	Client *const c = ecalloc(1, sizeof(Client));

	c->win = w;
	c->state.geometry.basic.x = wa->x;
	c->state.geometry.basic.y = wa->y;
	c->state.geometry.basic.w = wa->width;
	c->state.geometry.basic.h = wa->height;
	c->state.isfloating = 0;

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

	if (
		c->state.geometry.basic.x + WIDTH(c)
		>
		c->mon->screen_geometry.x + c->mon->screen_geometry.w
	) {
		c->state.geometry.basic.x =
			c->mon->screen_geometry.x + c->mon->screen_geometry.w - WIDTH(c);
	}

	if (
		c->state.geometry.basic.y + HEIGHT(c)
		>
		c->mon->screen_geometry.y + c->mon->screen_geometry.h
	) {
		c->state.geometry.basic.y =
			c->mon->screen_geometry.y + c->mon->screen_geometry.h - HEIGHT(c);
	}

	c->state.geometry.basic.x =
		MAX(c->state.geometry.basic.x, c->mon->screen_geometry.x);
	c->state.geometry.basic.y =
		MAX(c->state.geometry.basic.y, c->mon->screen_geometry.y);

	c->state.geometry.bw = settings_get_border_width();

	{
		XWindowChanges wc;
		wc.border_width = c->state.geometry.bw;
		XConfigureWindow(dpy, w, CWBorderWidth, &wc);
	}

	XSetWindowBorder(dpy, w, scheme[SchemeNorm][ColBorder].pixel);

	configure(c); /* propagates border_width, if size doesn't change */
	updatewindowtype(c);
	updatesizehints(c);
	updatewmhints(c);

	c->state.geometry.basic.x =
		c->mon->screen_geometry.x + (c->mon->screen_geometry.w - WIDTH(c)) / 2;
	c->state.geometry.basic.y =
		c->mon->screen_geometry.y + (c->mon->screen_geometry.h - HEIGHT(c)) / 2;

	XSelectInput(
		dpy,
		w,
		EnterWindowMask |
			FocusChangeMask |
			PropertyChangeMask |
			StructureNotifyMask
	);

	grabbuttons(c, 0);

	if (!c->state.isfloating) {
		c->state.isfloating = trans != None || c->state.isfixed;
	}

	if (c->state.isfloating) {
		XRaiseWindow(dpy, c->win);
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
		(unsigned char*)&(c->win),
		1
	);

	/* some windows require this */
	XMoveResizeWindow(
		dpy,
		c->win,
		c->state.geometry.basic.x + 2 * sw,
		c->state.geometry.basic.y,
		c->state.geometry.basic.w,
		c->state.geometry.basic.h
	);

	setclientstate(c, NormalState);

	if (c->mon == selmon) {
		unfocus(selmon->sel, 0);
	}

	c->mon->sel = c;

	arrange(c->mon);
	XMapWindow(dpy, c->win);
	focus(NULL);
}

void movemouse(__attribute__((unused)) const Arg *arg)
{
	Client *const c = selmon->sel;
	if (c == NULL) return;

	restack(selmon);

	if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
						None, cursor[CurMove]->cursor, CurrentTime)
			!= GrabSuccess)
	{
		return;
	}

	int x, y;
	if (!getrootptr(&x, &y)) return;

	const unsigned int snap_distance = settings_get_snap_distance();
	const int ocx = c->state.geometry.basic.x;
	const int ocy = c->state.geometry.basic.y;

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

			if (abs(selmon->window_area_geometry.x - nx) < snap_distance) {
				nx = selmon->window_area_geometry.x;
			} else if (
				abs(
					(
						selmon->window_area_geometry.x
						+
						selmon->window_area_geometry.w
					)
					-
					(nx + WIDTH(c))
				)
				<
				snap_distance
			) {
				nx = selmon->window_area_geometry.x + selmon->window_area_geometry.w - WIDTH(c);
			}

			if (abs(selmon->window_area_geometry.y - ny) < snap_distance) {
				ny = selmon->window_area_geometry.y;
			} else if (
				abs(
					(
						selmon->window_area_geometry.y
						+
						selmon->window_area_geometry.h
					) - (ny + HEIGHT(c))
				)
				<
				snap_distance
			) {
				ny =
					selmon->window_area_geometry.y
					+
					selmon->window_area_geometry.h
					-
					HEIGHT(c);
			}

			if (!c->state.isfloating &&
				(abs(nx - c->state.geometry.basic.x) > snap_distance ||
					abs(ny - c->state.geometry.basic.y) > snap_distance))
			{
				togglefloating(NULL);
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->state.isfloating) {
				resize(
					c,
					nx,
					ny,
					c->state.geometry.basic.w,
					c->state.geometry.basic.h,
					c->state.geometry.bw,
					1
				);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XUngrabPointer(dpy, CurrentTime);

	Monitor *const m = recttomon(
		c->state.geometry.basic.x,
		c->state.geometry.basic.y,
		c->state.geometry.basic.w,
		c->state.geometry.basic.h
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
			c && (!ISVISIBLE(c) || c->state.isfloating);
			c = c->next
		);

		if(!c) {
			for(
				c = selmon->clients;
				c && (!ISVISIBLE(c) || c->state.isfloating);
				c = c->next
			);
		}
	} else {
		/* find the client before selmon->sel */
		for (i = selmon->clients; i != selmon->sel; i = i->next) {
			if (ISVISIBLE(i) && !i->state.isfloating) {
				c = i;
			}
		}

		if (!c) {
			for (; i; i = i->next) {
				if (ISVISIBLE(i) && !i->state.isfloating) {
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
	for (; c && (c->state.isfloating || !ISVISIBLE(c)); c = c->next);
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
		if ((a = INTERSECT(x, y, w, h, m)) > area) {
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

void resize(Client *c, int x, int y, int w, int h, int bw, int interact)
{
	if (applysizehints(c, &x, &y, &w, &h, bw, interact))
		resizeclient(c, x, y, w, h, bw);
}

void resizeclient(Client *c, int x, int y, int w, int h, int bw)
{
	XWindowChanges wc;

	c->state.geometry.basic.x = wc.x = x;
	c->state.geometry.basic.y = wc.y = y;
	c->state.geometry.basic.w = wc.width = w;
	c->state.geometry.basic.h = wc.height = h;
	c->state.geometry.bw = wc.border_width = bw;

	XConfigureWindow(dpy, c->win, CWX|CWY|CWWidth|CWHeight|CWBorderWidth, &wc);
	configure(c);
	XSync(dpy, False);
}

void resizemouse(__attribute__((unused)) const Arg *arg)
{
	Client *const c = selmon->sel;
	if (c == NULL) return;

	restack(selmon);

	if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
						None, cursor[CurResize]->cursor, CurrentTime)
			!= GrabSuccess)
	{
		return;
	}

	XWarpPointer(
		dpy,
		None,
		c->win,
		0,
		0,
		0,
		0,
		c->state.geometry.basic.w + c->state.geometry.bw - 1,
		c->state.geometry.basic.h + c->state.geometry.bw - 1
	);

	const unsigned int snap_distance = settings_get_snap_distance();
	const int ocx = c->state.geometry.basic.x;
	const int ocy = c->state.geometry.basic.y;

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

			const int nw =
				MAX(ev.xmotion.x - ocx - 2 * c->state.geometry.bw + 1, 1);
			const int nh =
				MAX(ev.xmotion.y - ocy - 2 * c->state.geometry.bw + 1, 1);

			if (
				(
					c->mon->window_area_geometry.x + nw
					>=
					selmon->window_area_geometry.x
				)
				&&
				(
					c->mon->window_area_geometry.x + nw
					<=
					(
						selmon->window_area_geometry.x
						+
						selmon->window_area_geometry.w
					)
				)
				&&
				(
					c->mon->window_area_geometry.y + nh
					>=
					selmon->window_area_geometry.y
				)
				&&
				(
					c->mon->window_area_geometry.y + nh
					<=
					(
						selmon->window_area_geometry.y
						+
						selmon->window_area_geometry.h
					)
				)
			) {
				if (!c->state.isfloating &&
					(selmon->lt[selmon->sellt]->arrange == NULL ||
						abs(nw - c->state.geometry.basic.w) > snap_distance ||
						abs(nh - c->state.geometry.basic.h) > snap_distance))
				{
					togglefloating(NULL);
				}
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->state.isfloating) {
				resize(
					c,
					c->state.geometry.basic.x,
					c->state.geometry.basic.y,
					nw,
					nh,
					c->state.geometry.bw,
					1
				);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XWarpPointer(
		dpy,
		None,
		c->win,
		0,
		0,
		0,
		0,
		c->state.geometry.basic.w + c->state.geometry.bw - 1,
		c->state.geometry.basic.h + c->state.geometry.bw - 1
	);

	XUngrabPointer(dpy, CurrentTime);

	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));

	Monitor *const m = recttomon(
		c->state.geometry.basic.x,
		c->state.geometry.basic.y,
		c->state.geometry.basic.w,
		c->state.geometry.basic.h
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

	if (!m->sel)
		return;
	if (m->sel->state.isfloating || !m->lt[m->sellt]->arrange)
		XRaiseWindow(dpy, m->sel->win);
	if (m->lt[m->sellt]->arrange) {
		wc.stack_mode = Below;
		// TODO: Learn what is sibling and what
		// is the following line responsible for.
		// wc.sibling = m->bar->barwin;
		for (c = m->stack; c; c = c->snext)
			if (!c->state.isfloating && ISVISIBLE(c)) {
				XConfigureWindow(dpy, c->win, CWSibling|CWStackMode, &wc);
				wc.sibling = c->win;
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
	while (running && !XNextEvent(dpy, &ev))
		if (handler[ev.type])
			handler[ev.type](&ev); /* call handler */
}

void scan()
{
	unsigned int i, num;
	Window d1, d2, *wins = NULL;
	XWindowAttributes wa;

	if (XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
		for (i = 0; i < num; i++) {
			if (!XGetWindowAttributes(dpy, wins[i], &wa)
			|| wa.override_redirect || XGetTransientForHint(dpy, wins[i], &d1))
				continue;
			if (wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
				manage(wins[i], &wa);
		}
		for (i = 0; i < num; i++) { /* now the transients */
			if (!XGetWindowAttributes(dpy, wins[i], &wa))
				continue;
			if (XGetTransientForHint(dpy, wins[i], &d1)
			&& (wa.map_state == IsViewable || getstate(wins[i]) == IconicState))
				manage(wins[i], &wa);
		}
		if (wins)
			XFree(wins);
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
		c->win,
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

	if (XGetWMProtocols(dpy, c->win, &protocols, &n)) {
		while (!exists && n--)
			exists = protocols[n] == proto;
		XFree(protocols);
	}
	if (exists) {
		ev.type = ClientMessage;
		ev.xclient.window = c->win;
		ev.xclient.message_type = atoms->wmatom[WMProtocols];
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = proto;
		ev.xclient.data.l[1] = CurrentTime;
		XSendEvent(dpy, c->win, False, NoEventMask, &ev);
	}
	return exists;
}

void setfocus(Client *c)
{
	if (!c->state.neverfocus) {
		XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
		XChangeProperty(
			dpy,
			root,
			atoms->netatom[NetActiveWindow],
			XA_WINDOW,
			32,
			PropModeReplace,
			(unsigned char*)&(c->win),
			1
		);
	}

	sendevent(c, atoms->wmatom[WMTakeFocus]);
}

void setfullscreen(Client *c, int fullscreen)
{
	if (fullscreen && !c->state.isfullscreen) {
		XChangeProperty(dpy, c->win, atoms->netatom[NetWMState], XA_ATOM, 32,
			PropModeReplace, (unsigned char*)&atoms->netatom[NetWMFullscreen], 1);
		c->state.isfullscreen = 1;
		// We have to rearrange because borders and gaps may have changed.
		arrange(c->mon);
	} else if (!fullscreen && c->state.isfullscreen){
		XChangeProperty(dpy, c->win, atoms->netatom[NetWMState], XA_ATOM, 32,
			PropModeReplace, (unsigned char*)0, 0);
		c->state.isfullscreen = 0;
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

	/* clean up any zombies immediately */
	sigchld(0);

	if (!(global_unit = unit_new(UNIT_GLOBAL, NULL))) return false;

	/* init screen */
	screen = DefaultScreen(dpy);
	sw = DisplayWidth(dpy, screen);
	sh = DisplayHeight(dpy, screen);
	root = RootWindow(dpy, screen);
	drw = drw_create(dpy, screen, root, sw, sh);
	if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
		die("no fonts could be loaded.");
	updategeom();
	/* init atoms */
	atoms = atoms_create(dpy);
	if (atoms == NULL) die("polytreewm: fatal: cannot allocate atoms");
	/* init cursors */
	cursor[CurNormal] = drw_cur_create(drw, XC_left_ptr);
	cursor[CurResize] = drw_cur_create(drw, XC_sizing);
	cursor[CurMove] = drw_cur_create(drw, XC_fleur);
	/* init appearance */
	scheme = ecalloc(LENGTH(colors), sizeof(Clr *));
	for (unsigned int i = 0; i < LENGTH(colors); i++)
		scheme[i] = drw_scm_create(drw, colors[i], 3);

	/* supporting window for NetWMCheck */
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
		wm_name,
		strlen(wm_name)
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
	wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask
		|ButtonPressMask|PointerMotionMask|EnterWindowMask
		|LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;
	XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);
	XSelectInput(dpy, root, wa.event_mask);
	grabkeys();
	focus(NULL);

	return true;
}

void seturgent(Client *c, int urg)
{
	XWMHints *wmh;

	c->state.isurgent = urg;
	if (!(wmh = XGetWMHints(dpy, c->win)))
		return;
	wmh->flags = urg ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
	XSetWMHints(dpy, c->win, wmh);
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
			c->win,
			c->state.geometry.basic.x,
			c->state.geometry.basic.y
		);

		if (!c->mon->lt[c->mon->sellt]->arrange || c->state.isfloating) {
			resize(
				c,
				c->state.geometry.basic.x,
				c->state.geometry.basic.y,
				c->state.geometry.basic.w,
				c->state.geometry.basic.h,
				c->state.geometry.bw,
				0
			);
		}
		showhide(c->snext);
	} else {
		/* hide clients bottom up */
		showhide(c->snext);
		XMoveWindow(dpy, c->win, WIDTH(c) * -2, c->state.geometry.basic.y);
	}
}

void sigchld(__attribute__((unused)) int unused)
{
	if (signal(SIGCHLD, sigchld) == SIG_ERR)
		die("can't install SIGCHLD handler:");
	while (0 < waitpid(-1, NULL, WNOHANG));
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

	selmon->sel->state.isfloating = !selmon->sel->state.isfloating || selmon->sel->state.isfixed;

	const int border_width = settings_get_border_width();

	if (selmon->sel->state.isfloating) {
		resize(
			selmon->sel,
			selmon->sel->state.geometry.basic.x,
			selmon->sel->state.geometry.basic.y,
			selmon->sel->state.geometry.basic.w -
				2 * (border_width - selmon->sel->state.geometry.bw),
			selmon->sel->state.geometry.basic.h -
				2 * (border_width - selmon->sel->state.geometry.bw),
			border_width,
			0
		);
	}

	arrange(selmon);
}

void unfocus(Client *c, int setfocus)
{
	if (!c)
		return;
	grabbuttons(c, 0);
	XSetWindowBorder(dpy, c->win, scheme[SchemeNorm][ColBorder].pixel);
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
		XConfigureWindow(dpy, c->win, CWBorderWidth, &wc); /* remove border */
		XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
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
				(unsigned char*)&(c->win),
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
					m->next = createmon();
				else
					mons = createmon();
			}
			for (i = 0, m = mons; i < nn && m; m = m->next, i++)
				if (
					i >= n
					||
					unique[i].x_org != m->screen_geometry.x
					||
					unique[i].y_org != m->screen_geometry.y
					||
					unique[i].width != m->screen_geometry.w
					||
					unique[i].height != m->screen_geometry.h
				) {
					dirty = 1;
					m->num = i;
					m->screen_geometry.x =
						m->window_area_geometry.x =
						unique[i].x_org;
					m->screen_geometry.y =
						m->window_area_geometry.y =
						unique[i].y_org;
					m->screen_geometry.w =
						m->window_area_geometry.w =
						unique[i].width;
					m->screen_geometry.h =
						m->window_area_geometry.h =
						unique[i].height;
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
				cleanupmon(m);
			}
		}
		free(unique);
	} else
#endif /* ENABLE_XINERAMA */
	{ /* default monitor setup */
		if (!mons)
			mons = createmon();
		if (mons->screen_geometry.w != sw || mons->screen_geometry.h != sh) {
			dirty = 1;
			mons->screen_geometry.w = mons->window_area_geometry.w = sw;
			mons->screen_geometry.h = mons->window_area_geometry.h = sh;
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
	for (i = 0; i < 8; i++)
		for (int j = 0; j < modmap->max_keypermod; j++)
			if (modmap->modifiermap[i * modmap->max_keypermod + j]
				== XKeysymToKeycode(dpy, XK_Num_Lock))
				numlockmask = (1 << i);
	XFreeModifiermap(modmap);
}

void updatesizehints(Client *c)
{
	XSizeHints size = { 0 };
	long msize = 0;

	if (!XGetWMNormalHints(dpy, c->win, &size, &msize)) {
		/* size is uninitialized, ensure that size.flags aren't used */
		size.flags = PSize;
	}

	if (size.flags & PBaseSize) {
		c->size_hints.basew = size.base_width;
		c->size_hints.baseh = size.base_height;
	} else if (size.flags & PMinSize) {
		c->size_hints.basew = size.min_width;
		c->size_hints.baseh = size.min_height;
	} else {
		c->size_hints.basew = c->size_hints.baseh = 0;
	}

	if (size.flags & PResizeInc) {
		c->size_hints.incw = size.width_inc;
		c->size_hints.inch = size.height_inc;
	} else {
		c->size_hints.incw = c->size_hints.inch = 0;
	}

	if (size.flags & PMaxSize) {
		c->size_hints.maxw = size.max_width;
		c->size_hints.maxh = size.max_height;
	} else {
		c->size_hints.maxw = c->size_hints.maxh = 0;
	}

	if (size.flags & PMinSize) {
		c->size_hints.minw = size.min_width;
		c->size_hints.minh = size.min_height;
	} else if (size.flags & PBaseSize) {
		c->size_hints.minw = size.base_width;
		c->size_hints.minh = size.base_height;
	} else {
		c->size_hints.minw = c->size_hints.minh = 0;
	}

	if (size.flags & PAspect) {
		c->size_hints.mina = (float)size.min_aspect.y / size.min_aspect.x;
		c->size_hints.maxa = (float)size.max_aspect.x / size.max_aspect.y;
	} else {
		c->size_hints.maxa = c->size_hints.mina = 0.0;
	}

	c->state.isfixed = (
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
	if (!gettextprop(c->win, atoms->netatom[NetWMName], c->state.name, sizeof(c->state.name)))
		gettextprop(c->win, XA_WM_NAME, c->state.name, sizeof(c->state.name));
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
		c->state.isfloating = 1;
}

void updatewmhints(Client *c)
{
	XWMHints *wmh = XGetWMHints(dpy, c->win);
	if (!wmh) return;

	if (c == selmon->sel && wmh->flags & XUrgencyHint) {
		wmh->flags &= ~XUrgencyHint;
		XSetWMHints(dpy, c->win, wmh);
	} else {
		c->state.isurgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
	}

	if (wmh->flags & InputHint) {
		c->state.neverfocus = !wmh->input;
	} else {
		c->state.neverfocus = 0;
	}

	XFree(wmh);
}

Client *wintoclient(Window w)
{
	for (Monitor *m = mons; m; m = m->next) {
		for (Client *c = m->clients; c; c = c->next) {
			if (c->win == w) {
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

void zoom(__attribute__((unused)) const Arg *arg)
{
	Client *c = selmon->sel;

	if (!selmon->lt[selmon->sellt]->arrange
	|| (selmon->sel && selmon->sel->state.isfloating))
		return;
	if (c == nexttiled(selmon->clients))
		if (!c || !(c = nexttiled(c->next)))
			return;
	pop(c);
}
