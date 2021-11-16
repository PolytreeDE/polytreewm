/* See LICENSE file for copyright and license details.
 *
 * Polytree tiling window manager is designed like any other X client as well.
 * It is driven through handling X events. In contrast to other X clients, a
 * window manager selects for SubstructureRedirectMask on the root window, to
 * receive events about window (dis-)appearance. Only one X connection at a time
 * is allowed to select for this event mask.
 *
 * The event handlers of PolytreeWM are organized in an array which is accessed
 * whenever a new event has been fetched. This allows event dispatching
 * in O(1) time.
 *
 * Each child of the root window is called a client, except windows which have
 * set the override_redirect flag. Clients are organized in a linked client
 * list on each monitor, the focus history is remembered through a stack list
 * on each monitor. Each client contains a bit array to indicate the tags of a
 * client.
 *
 * Keys and tagging rules are organized as arrays and defined in config.h.
 *
 * To understand everything else, start reading main().
 */
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */
#include <X11/Xft/Xft.h>
#include <X11/Xlib-xcb.h>
#include <xcb/res.h>
#ifdef __OpenBSD__
#include <sys/sysctl.h>
#include <kvm.h>
#endif /* __OpenBSD */

#include "atoms.h"
#include "drw.h"
#include "helpers.h"
#include "layouts.h"
#include "settings.h"
#include "spawn.h"
#include "tags.h"
#include "util.h"

/* macros */
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)         (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x,y,w,h,m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                               * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define ISVISIBLE(C)            ((C->tags & C->mon->tagset[C->mon->seltags]))
#define LENGTH(X)               (sizeof X / sizeof X[0])
#define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)
#define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)

#define SYSTEM_TRAY_REQUEST_DOCK    0

/* XEMBED messages */
#define XEMBED_EMBEDDED_NOTIFY      0
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_FOCUS_IN             4
#define XEMBED_MODALITY_ON         10

#define XEMBED_MAPPED              (1 << 0)
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_WINDOW_DEACTIVATE    2

#define VERSION_MAJOR               0
#define VERSION_MINOR               0
#define XEMBED_EMBEDDED_VERSION (VERSION_MAJOR << 16) | VERSION_MINOR

/*********
 * types *
 *********/

enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel }; /* color schemes */
enum { ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
       ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

typedef struct Monitor Monitor;
typedef struct Client Client;
typedef struct Pertag Pertag;
typedef struct Systray Systray;

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
	char name[256];
	float mina, maxa;
	int x, y, w, h;
	int oldx, oldy, oldw, oldh;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
	int bw, oldbw;
	unsigned int tags;
	int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen, isterminal, noswallow;
	pid_t pid;
	Client *next;
	Client *snext;
	Client *swallowing;
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
	char ltsymbol[16];
	float mfact;
	int nmaster;
	int num;
	int by;               /* bar geometry */
	int mx, my, mw, mh;   /* screen size */
	int wx, wy, ww, wh;   /* window area  */
	unsigned int seltags;
	unsigned int sellt;
	unsigned int tagset[2];
	bool showbar;
	int topbar;
	Client *clients;
	Client *sel;
	Client *stack;
	Monitor *next;
	Window barwin;
	const Layout *lt[2];
	Pertag *pertag;
};

typedef struct {
	const char *class;
	const char *instance;
	const char *title;
	unsigned int tags;
	int isfloating;
	int isterminal;
	int noswallow;
	int monitor;
} Rule;

struct Systray {
	Window win;
	Client *icons;
};

struct Pertag {
	unsigned int curtag, prevtag; /* current and previous tag */
	int nmasters[TAGS_COUNT + 1]; /* number of windows in master area */
	float mfacts[TAGS_COUNT + 1]; /* mfacts per tag */
	unsigned int sellts[TAGS_COUNT + 1]; /* selected layouts */
	const Layout *ltidxs[TAGS_COUNT + 1][2]; /* matrix of tags and layouts indexes  */
	bool showbars[TAGS_COUNT + 1]; /* display bar for the current tag */
};

/*************************
 * function declarations *
 *************************/

static void applyrules(Client *c);
static int applysizehints(Client *c, int *x, int *y, int *w, int *h, int bw, int interact);
static void arrange(Monitor *m);
static void arrangemon(Monitor *m);
static void attach(Client *c);
static void attachstack(Client *c);
static void configborder(const Arg *arg);
static void configgap(const Arg *arg);
static void checkotherwm(void);
static void cleanup(void);
static void cleanupmon(Monitor *mon);
static void configure(Client *c);
static Monitor *createmon(void);
static void detach(Client *c);
static void detachstack(Client *c);
static Monitor *dirtomon(int dir);
static void drawbar(Monitor *m);
static void drawbars(void);
static void focus(Client *c);
static void focusmon(const Arg *arg);
static void focusstack(const Arg *arg);
static Atom getatomprop(Client *c, Atom prop);
static int getrootptr(int *x, int *y);
static long getstate(Window w);
static int gettextprop(Window w, Atom atom, char *text, unsigned int size);
static void grabbuttons(Client *c, int focused);
static void grabkeys(void);
static void incnmaster(const Arg *arg);
static void killclient(const Arg *arg);
static void manage(Window w, XWindowAttributes *wa);
static void movemouse(const Arg *arg);
static void movestack(const Arg *arg);
static void nametag(const Arg *arg);
static Client *nexttiled(Client *c);
static void pop(Client *);
static void quit(const Arg *arg);
static Monitor *recttomon(int x, int y, int w, int h);
static void resetnmaster(const Arg *arg);
static void resize(Client *c, int x, int y, int w, int h, int bw, int interact);
static void resizebarwin(Monitor *m);
static void resizeclient(Client *c, int x, int y, int w, int h, int bw);
static void resizemouse(const Arg *arg);
static void restack(Monitor *m);
static void run(void);
static void scan(void);
static int sendevent(Window w, Atom proto, int m, long d0, long d1, long d2, long d3, long d4);
static void sendmon(Client *c, Monitor *m);
static void setclientstate(Client *c, long state);
static void setfocus(Client *c);
static void setfullscreen(Client *c, int fullscreen);
static void setlayout(const Arg *arg);
static void setmfact(const Arg *arg);
static void setup(void);
static void seturgent(Client *c, int urg);
static void showhide(Client *c);
static void sigchld(int unused);
static void spawn(const Arg *arg);
static void spawn_callback();
static void tag(const Arg *arg);
static void tagmon(const Arg *arg);
static void togglebar(const Arg *arg);
static void togglefloating(const Arg *arg);
static void toggletag(const Arg *arg);
static void toggleview(const Arg *arg);
static void unfocus(Client *c, int setfocus);
static void unmanage(Client *c, int destroyed);
static void updatebarpos(Monitor *m);
static void updatebars(void);
static void updateclientlist(void);
static int updategeom(void);
static void updatenumlockmask(void);
static void updatesizehints(Client *c);
static void updatestatus(void);
static void updatetitle(Client *c);
static void updatewindowtype(Client *c);
static void updatewmhints(Client *c);
static void view(const Arg *arg);
static void viewrel(const Arg *arg);
static Client *wintoclient(Window w);
static Monitor *wintomon(Window w);
static int xerror(Display *dpy, XErrorEvent *ee);
static int xerrordummy(Display *dpy, XErrorEvent *ee);
static int xerrorstart(Display *dpy, XErrorEvent *ee);
static void zoom(const Arg *arg);

#include "dwm/handlers.h"
#include "dwm/layouts.h"
#include "dwm/swallow.h"
#include "dwm/systray.h"

/*************
 * variables *
 *************/

static Systray *systray =  NULL;
static const char broken[] = "broken";
static char stext[256];
static int screen;
static int sw, sh;           /* X display screen geometry width, height */
static int bh, blw = 0;      /* bar geometry */
static int lrpad;            /* sum of left and right padding for text */
static int (*xerrorxlib)(Display *, XErrorEvent *);
static unsigned int numlockmask = 0;
static void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = on_button_press,
	[ClientMessage] = on_client_message,
	[ConfigureRequest] = on_configure_request,
	[ConfigureNotify] = on_configure_notify,
	[DestroyNotify] = on_destroy_notify,
	[Expose] = on_expose,
	[FocusIn] = on_focus_in,
	[KeyPress] = on_key_press,
	[MappingNotify] = on_mapping_notify,
	[MapRequest] = on_map_request,
	[PropertyNotify] = on_property_notify,
	[ResizeRequest] = on_resize_request,
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

static xcb_connection_t *xcon;

/***************************************************************
 * configuration, allows nested code to access above variables *
 ***************************************************************/

#include "config.h"

/****************************
 * function implementations *
 ****************************/

#include "dwm/handlers.c"
#include "dwm/layouts.c"
#include "dwm/swallow.c"
#include "dwm/systray.c"

int
main(int argc, char *argv[])
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

	if (!(xcon = XGetXCBConnection(dpy))) {
		die("polytreewm: cannot get xcb connection");
	}

	checkotherwm();

	setup();

#ifdef __OpenBSD__
	if (pledge("stdio rpath proc exec ps", NULL) == -1) {
		die("pledge");
	}
#endif /* __OpenBSD__ */

	scan();
	run();
	cleanup();
	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}

void
applyrules(Client *c)
{
	const char *class, *instance;
	unsigned int i;
	const Rule *r;
	Monitor *m;
	XClassHint ch = { NULL, NULL };

	/* rule matching */
	c->isfloating = 0;
	c->tags = 0;
	XGetClassHint(dpy, c->win, &ch);
	class    = ch.res_class ? ch.res_class : broken;
	instance = ch.res_name  ? ch.res_name  : broken;

	for (i = 0; i < LENGTH(rules); i++) {
		r = &rules[i];
		if ((!r->title || strstr(c->name, r->title))
		&& (!r->class || strstr(class, r->class))
		&& (!r->instance || strstr(instance, r->instance)))
		{
			c->isterminal = r->isterminal;
			c->noswallow  = r->noswallow;
			c->isfloating = r->isfloating;
			c->tags |= r->tags;
			for (m = mons; m && m->num != r->monitor; m = m->next);
			if (m)
				c->mon = m;
		}
	}
	if (ch.res_class)
		XFree(ch.res_class);
	if (ch.res_name)
		XFree(ch.res_name);
	c->tags = c->tags & TAGS_MASK ? c->tags & TAGS_MASK : c->mon->tagset[c->mon->seltags];
}

int
applysizehints(Client *c, int *x, int *y, int *w, int *h, int bw, int interact)
{
	int baseismin;
	Monitor *m = c->mon;

	/* set minimum possible */
	*w = MAX(1, *w);
	*h = MAX(1, *h);
	if (interact) {
		if (*x > sw)
			*x = sw - WIDTH(c);
		if (*y > sh)
			*y = sh - HEIGHT(c);
		if (*x + *w + 2 * bw < 0)
			*x = 0;
		if (*y + *h + 2 * bw < 0)
			*y = 0;
	} else {
		if (*x >= m->wx + m->ww)
			*x = m->wx + m->ww - WIDTH(c);
		if (*y >= m->wy + m->wh)
			*y = m->wy + m->wh - HEIGHT(c);
		if (*x + *w + 2 * bw <= m->wx)
			*x = m->wx;
		if (*y + *h + 2 * bw <= m->wy)
			*y = m->wy;
	}
	if (*h < bh)
		*h = bh;
	if (*w < bh)
		*w = bh;
	if (
		c->isfloating
		||
		(
			settings_get_respect_resize_hints_in_floating_layout()
			&&
			!c->mon->lt[c->mon->sellt]->arrange
		)
	) {
		/* see last two sentences in ICCCM 4.1.2.3 */
		baseismin = c->basew == c->minw && c->baseh == c->minh;
		if (!baseismin) { /* temporarily remove base dimensions */
			*w -= c->basew;
			*h -= c->baseh;
		}
		/* adjust for aspect limits */
		if (c->mina > 0 && c->maxa > 0) {
			if (c->maxa < (float)*w / *h)
				*w = *h * c->maxa + 0.5;
			else if (c->mina < (float)*h / *w)
				*h = *w * c->mina + 0.5;
		}
		if (baseismin) { /* increment calculation requires this */
			*w -= c->basew;
			*h -= c->baseh;
		}
		/* adjust for increment value */
		if (c->incw)
			*w -= *w % c->incw;
		if (c->inch)
			*h -= *h % c->inch;
		/* restore base dimensions */
		*w = MAX(*w + c->basew, c->minw);
		*h = MAX(*h + c->baseh, c->minh);
		if (c->maxw)
			*w = MIN(*w, c->maxw);
		if (c->maxh)
			*h = MIN(*h, c->maxh);
	}
	return *x != c->x || *y != c->y || *w != c->w || *h != c->h || bw != c->bw;
}

void
arrange(Monitor *m)
{
	if (m)
		showhide(m->stack);
	else for (m = mons; m; m = m->next)
		showhide(m->stack);
	if (m) {
		arrangemon(m);
		restack(m);
	} else for (m = mons; m; m = m->next)
		arrangemon(m);
}

void
arrangemon(Monitor *m)
{
	unsigned int visible_clients = 0;
	for (const Client *client = m->clients; client; client = client->next) {
		if (ISVISIBLE(client)) ++visible_clients;
	}

	layouts_symbol_func(
		m->lt[m->sellt]->symbol_func,
		m->ltsymbol,
		sizeof(m->ltsymbol),
		m->nmaster,
		visible_clients
	);

	if (m->lt[m->sellt]->arrange)
		m->lt[m->sellt]->arrange(m);
	else {
		const int border_width = settings_get_border_width();

		/* <>< case; rather than providing an arrange function and upsetting other logic that tests for its presence, simply add borders here */
		for (Client *c = selmon->clients; c; c = c->next)
			if (ISVISIBLE(c) && c->bw == 0)
				resize(
					c,
					c->x,
					c->y,
					c->w - 2 * border_width,
					c->h - 2 * border_width,
					border_width,
					0
				);
	}
}

void
attach(Client *c)
{
	c->next = c->mon->clients;
	c->mon->clients = c;
}

void
attachstack(Client *c)
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

void
checkotherwm(void)
{
	xerrorxlib = XSetErrorHandler(xerrorstart);
	/* this causes an error if some other window manager is running */
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
	XSync(dpy, False);
	XSetErrorHandler(xerror);
	XSync(dpy, False);
}

void
cleanup(void)
{
	Arg a = {.ui = ~0};
	Layout foo = { NULL, NULL };
	Monitor *m;
	size_t i;

	view(&a);
	selmon->lt[selmon->sellt] = &foo;
	for (m = mons; m; m = m->next)
		while (m->stack)
			unmanage(m->stack, 0);
	XUngrabKey(dpy, AnyKey, AnyModifier, root);
	while (mons)
		cleanupmon(mons);
	if (showsystray) {
		XUnmapWindow(dpy, systray->win);
		XDestroyWindow(dpy, systray->win);
		free(systray);
	}
	for (i = 0; i < CurLast; i++)
		drw_cur_free(drw, cursor[i]);
	for (i = 0; i < LENGTH(colors); i++)
		free(scheme[i]);
	atoms_destroy(atoms);
	XDestroyWindow(dpy, wmcheckwin);
	drw_free(drw);
	XSync(dpy, False);
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XDeleteProperty(dpy, root, atoms->netatom[NetActiveWindow]);
}

void
cleanupmon(Monitor *mon)
{
	Monitor *m;

	if (mon == mons)
		mons = mons->next;
	else {
		for (m = mons; m && m->next != mon; m = m->next);
		m->next = mon->next;
	}
	XUnmapWindow(dpy, mon->barwin);
	XDestroyWindow(dpy, mon->barwin);
	free(mon);
}

void
configure(Client *c)
{
	XConfigureEvent ce;

	ce.type = ConfigureNotify;
	ce.display = dpy;
	ce.event = c->win;
	ce.window = c->win;
	ce.x = c->x;
	ce.y = c->y;
	ce.width = c->w;
	ce.height = c->h;
	ce.border_width = c->bw;
	ce.above = None;
	ce.override_redirect = False;
	XSendEvent(dpy, c->win, False, StructureNotifyMask, (XEvent *)&ce);
}

Monitor *
createmon(void)
{
	Monitor *m;
	unsigned int i;

	m = ecalloc(1, sizeof(Monitor));
	m->tagset[0] = m->tagset[1] = 1;
	m->mfact = mfact;
	m->nmaster = settings_get_default_clients_in_master();
	m->showbar = settings_get_show_bar_by_default();
	m->topbar = topbar;
	m->lt[0] = &layouts[0];
	m->lt[1] = &layouts[1 % LENGTH(layouts)];

	layouts_symbol_func(
		layouts[0].symbol_func,
		m->ltsymbol,
		sizeof(m->ltsymbol),
		0,
		0
	);

	m->pertag = ecalloc(1, sizeof(Pertag));
	m->pertag->curtag = m->pertag->prevtag = 1;

	for (i = 0; i <= TAGS_COUNT; i++) {
		m->pertag->nmasters[i] = m->nmaster;
		m->pertag->mfacts[i] = m->mfact;

		m->pertag->ltidxs[i][0] = m->lt[0];
		m->pertag->ltidxs[i][1] = m->lt[1];
		m->pertag->sellts[i] = m->sellt;

		m->pertag->showbars[i] = m->showbar;
	}

	return m;
}

void
detach(Client *c)
{
	Client **tc;

	for (tc = &c->mon->clients; *tc && *tc != c; tc = &(*tc)->next);
	*tc = c->next;
}

void
detachstack(Client *c)
{
	Client **tc, *t;

	for (tc = &c->mon->stack; *tc && *tc != c; tc = &(*tc)->snext);
	*tc = c->snext;

	if (c == c->mon->sel) {
		for (t = c->mon->stack; t && !ISVISIBLE(t); t = t->snext);
		c->mon->sel = t;
	}
}

Monitor *
dirtomon(int dir)
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

void
drawbar(Monitor *m)
{
	int x, w, tw = 0, stw = 0;
	int boxs = drw->fonts->h / 9;
	int boxw = drw->fonts->h / 6 + 2;
	unsigned int i, occ = 0, urg = 0;
	Client *c;

	if(showsystray && m == systraytomon(m) && !systrayonleft)
		stw = getsystraywidth();

	/* draw status first so it can be overdrawn by tags later */
	drw_setscheme(drw, scheme[SchemeNorm]);
	tw = TEXTW(stext) - lrpad / 2 + 2; /* 2px extra right padding */
	drw_text(drw, m->ww - tw - stw, 0, tw, bh, lrpad / 2 - 2, stext, 0);

	resizebarwin(m);
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
		drw_text(drw, x, 0, w, bh, lrpad / 2, tags_get(i)->name.cstr, urg & 1 << i);
		x += w;
	}
	w = blw = TEXTW(m->ltsymbol);
	drw_setscheme(drw, scheme[SchemeNorm]);
	x = drw_text(drw, x, 0, w, bh, lrpad / 2, m->ltsymbol, 0);

	if ((w = m->ww - tw - stw - x) > bh) {
		if (m->sel) {
			drw_setscheme(drw, scheme[m == selmon ? SchemeSel : SchemeNorm]);
			drw_text(drw, x, 0, w, bh, lrpad / 2, m->sel->name, 0);
			if (m->sel->isfloating)
				drw_rect(drw, x + boxs, boxs, boxw, boxw, m->sel->isfixed, 0);
		} else {
			drw_setscheme(drw, scheme[SchemeNorm]);
			drw_rect(drw, x, 0, w, bh, 1, 1);
		}
	}
	drw_map(drw, m->barwin, 0, 0, m->ww - stw, bh);
}

void
drawbars(void)
{
	Monitor *m;

	for (m = mons; m; m = m->next)
		drawbar(m);
}

void
focus(Client *c)
{
	if (!c || !ISVISIBLE(c))
		for (c = selmon->stack; c && !ISVISIBLE(c); c = c->snext);
	if (selmon->sel && selmon->sel != c)
		unfocus(selmon->sel, 0);
	if (c) {
		if (c->mon != selmon)
			selmon = c->mon;
		if (c->isurgent)
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
	drawbars();
}

void
focusmon(const Arg *arg)
{
	Monitor *m;

	if (!mons->next)
		return;
	if ((m = dirtomon(arg->i)) == selmon)
		return;
	unfocus(selmon->sel, 0);
	selmon = m;
	focus(NULL);
}

void
focusstack(const Arg *arg)
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

Atom
getatomprop(Client *c, Atom prop)
{
	int di;
	unsigned long dl;
	unsigned char *p = NULL;
	Atom da, atom = None;
	/* FIXME getatomprop should return the number of items and a pointer to
	 * the stored data instead of this workaround */
	Atom req = XA_ATOM;
	if (prop == atoms->xatom[XembedInfo])
		req = atoms->xatom[XembedInfo];

	if (XGetWindowProperty(dpy, c->win, prop, 0L, sizeof atom, False, req,
		&da, &di, &dl, &dl, &p) == Success && p) {
		atom = *(Atom *)p;
		if (da == atoms->xatom[XembedInfo] && dl == 2)
			atom = ((Atom *)p)[1];
		XFree(p);
	}
	return atom;
}

int
getrootptr(int *x, int *y)
{
	int di;
	unsigned int dui;
	Window dummy;

	return XQueryPointer(dpy, root, &dummy, &dummy, x, y, &di, &di, &dui);
}

long
getstate(Window w)
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

int
gettextprop(Window w, Atom atom, char *text, unsigned int size)
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

void
grabbuttons(Client *c, int focused)
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

void
grabkeys(void)
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

void
incnmaster(const Arg *arg)
{
	const int max_clients_in_master = settings_get_max_clients_in_master();
	const int new_clients_in_master = MAX(0, selmon->nmaster + arg->i);

	selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag] =
		max_clients_in_master == 0
		? new_clients_in_master
		: MIN(new_clients_in_master, max_clients_in_master);

	arrange(selmon);
}

#ifdef XINERAMA
static int
isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info)
{
	while (n--)
		if (unique[n].x_org == info->x_org && unique[n].y_org == info->y_org
		&& unique[n].width == info->width && unique[n].height == info->height)
			return 0;
	return 1;
}
#endif /* XINERAMA */

void
killclient(const Arg *arg)
{
	if (!selmon->sel)
		return;
	if (!sendevent(selmon->sel->win, atoms->wmatom[WMDelete], NoEventMask, atoms->wmatom[WMDelete], CurrentTime, 0 , 0, 0)) {
		XGrabServer(dpy);
		XSetErrorHandler(xerrordummy);
		XSetCloseDownMode(dpy, DestroyAll);
		XKillClient(dpy, selmon->sel->win);
		XSync(dpy, False);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
}

void
manage(Window w, XWindowAttributes *wa)
{
	Client *const c = ecalloc(1, sizeof(Client));

	c->win = w;
	c->pid = winpid(w);
	c->x = c->oldx = wa->x;
	c->y = c->oldy = wa->y;
	c->w = c->oldw = wa->width;
	c->h = c->oldh = wa->height;
	c->oldbw = wa->border_width;

	updatetitle(c);

	Window trans = None;
	Client *term = NULL;

	{
		Client *t = NULL;

		if (XGetTransientForHint(dpy, w, &trans) && (t = wintoclient(trans))) {
			c->mon = t->mon;
			c->tags = t->tags;
		} else {
			c->mon = selmon;
			applyrules(c);
			term = termforwin(c);
		}
	}

	if (c->x + WIDTH(c) > c->mon->mx + c->mon->mw) {
		c->x = c->mon->mx + c->mon->mw - WIDTH(c);
	}

	if (c->y + HEIGHT(c) > c->mon->my + c->mon->mh) {
		c->y = c->mon->my + c->mon->mh - HEIGHT(c);
	}

	c->x = MAX(c->x, c->mon->mx);

	/* only fix client y-offset, if the client center might cover the bar */
	c->y = MAX(
		c->y,
		(
			(c->mon->by == c->mon->my) &&
			(c->x + (c->w / 2) >= c->mon->wx) &&
			(c->x + (c->w / 2) < c->mon->wx + c->mon->ww)
		)
			? bh
			: c->mon->my
	);

	c->bw = settings_get_border_width();

	{
		XWindowChanges wc;
		wc.border_width = c->bw;
		XConfigureWindow(dpy, w, CWBorderWidth, &wc);
	}

	XSetWindowBorder(dpy, w, scheme[SchemeNorm][ColBorder].pixel);

	configure(c); /* propagates border_width, if size doesn't change */
	updatewindowtype(c);
	updatesizehints(c);
	updatewmhints(c);

	c->x = c->mon->mx + (c->mon->mw - WIDTH(c)) / 2;
	c->y = c->mon->my + (c->mon->mh - HEIGHT(c)) / 2;

	XSelectInput(
		dpy,
		w,
		EnterWindowMask |
			FocusChangeMask |
			PropertyChangeMask |
			StructureNotifyMask
	);

	grabbuttons(c, 0);

	if (!c->isfloating) {
		c->isfloating = c->oldstate = trans != None || c->isfixed;
	}

	if (c->isfloating) {
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
	XMoveResizeWindow(dpy, c->win, c->x + 2 * sw, c->y, c->w, c->h);

	setclientstate(c, NormalState);

	if (c->mon == selmon) {
		unfocus(selmon->sel, 0);
	}

	c->mon->sel = c;

	arrange(c->mon);

	XMapWindow(dpy, c->win);

	if (term) swallow(term, c);

	focus(NULL);
}

void
movemouse(const Arg *arg)
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
	const int ocx = c->x;
	const int ocy = c->y;

	Time lasttime = 0;

	XEvent ev;

	do {
		XMaskEvent(dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);

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

			if (abs(selmon->wx - nx) < snap_distance) {
				nx = selmon->wx;
			} else if (abs((selmon->wx + selmon->ww) - (nx + WIDTH(c))) < snap_distance) {
				nx = selmon->wx + selmon->ww - WIDTH(c);
			}

			if (abs(selmon->wy - ny) < snap_distance) {
				ny = selmon->wy;
			} else if (abs((selmon->wy + selmon->wh) - (ny + HEIGHT(c))) < snap_distance) {
				ny = selmon->wy + selmon->wh - HEIGHT(c);
			}

			if (!c->isfloating &&
				(abs(nx - c->x) > snap_distance ||
					abs(ny - c->y) > snap_distance))
			{
				togglefloating(NULL);
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->isfloating) {
				resize(c, nx, ny, c->w, c->h, c->bw, 1);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XUngrabPointer(dpy, CurrentTime);

	Monitor *const m = recttomon(c->x, c->y, c->w, c->h);

	if (m != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
}

// TODO: this function really needs to be refactored
void
nametag(const Arg *arg) {
	char *p, name[TAGS_CUSTOM_NAME_SIZE];
	FILE *f;
	int i;

	errno = 0; // popen(3p) says on failure it "may" set errno
	if(!(f = popen("rofi -dmenu -p \"Tag name\"", "r"))) {
		fprintf(stderr, "polytreewm: popen 'rofi -dmenu -p \"Tag name\"' failed%s%s\n", errno ? ": " : "", errno ? strerror(errno) : "");
		return;
	}
	if (!(p = fgets(name, TAGS_CUSTOM_NAME_SIZE, f)) && (i = errno) && ferror(f))
		fprintf(stderr, "polytreewm: fgets failed: %s\n", strerror(i));
	if (pclose(f) < 0)
		fprintf(stderr, "polytreewm: pclose failed: %s\n", strerror(errno));
	if(!p)
		return;
	if((p = strchr(name, '\n')))
		*p = '\0';

	for (i = 0; i < TAGS_COUNT; ++i) {
		if (selmon->tagset[selmon->seltags] & (1 << i)) {
			tags_rename(i, name);
		}
	}

	drawbars();
}

void
movestack(const Arg *arg) {
	Client *c = NULL, *p = NULL, *pc = NULL, *i;

	if(arg->i > 0) {
		/* find the client after selmon->sel */
		for(c = selmon->sel->next; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);
		if(!c)
			for(c = selmon->clients; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);

	}
	else {
		/* find the client before selmon->sel */
		for(i = selmon->clients; i != selmon->sel; i = i->next)
			if(ISVISIBLE(i) && !i->isfloating)
				c = i;
		if(!c)
			for(; i; i = i->next)
				if(ISVISIBLE(i) && !i->isfloating)
					c = i;
	}
	/* find the client before selmon->sel and c */
	for(i = selmon->clients; i && (!p || !pc); i = i->next) {
		if(i->next == selmon->sel)
			p = i;
		if(i->next == c)
			pc = i;
	}

	/* swap c and selmon->sel selmon->clients in the selmon->clients list */
	if(c && c != selmon->sel) {
		Client *temp = selmon->sel->next==c?selmon->sel:selmon->sel->next;
		selmon->sel->next = c->next==selmon->sel?c:c->next;
		c->next = temp;

		if(p && p != c)
			p->next = c;
		if(pc && pc != selmon->sel)
			pc->next = selmon->sel;

		if(selmon->sel == selmon->clients)
			selmon->clients = c;
		else if(c == selmon->clients)
			selmon->clients = selmon->sel;

		arrange(selmon);
	}
}

Client *
nexttiled(Client *c)
{
	for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next);
	return c;
}

void
pop(Client *c)
{
	detach(c);
	attach(c);
	focus(c);
	arrange(c->mon);
}

void
quit(const Arg *arg)
{
	running = 0;
}

Monitor *
recttomon(int x, int y, int w, int h)
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

void
resetnmaster(const Arg *arg)
{
	const int max_clients_in_master = settings_get_max_clients_in_master();
	const int new_clients_in_master = arg->i == 0 ? 0 : settings_get_default_clients_in_master();

	selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag] =
		max_clients_in_master == 0
		? new_clients_in_master
		: MIN(new_clients_in_master, max_clients_in_master);

	arrange(selmon);
}

void
resize(Client *c, int x, int y, int w, int h, int bw, int interact)
{
	if (applysizehints(c, &x, &y, &w, &h, bw, interact))
		resizeclient(c, x, y, w, h, bw);
}

void
resizebarwin(Monitor *m) {
	unsigned int w = m->ww;
	if (showsystray && m == systraytomon(m) && !systrayonleft)
		w -= getsystraywidth();
	XMoveResizeWindow(dpy, m->barwin, m->wx, m->by, w, bh);
}

void
resizeclient(Client *c, int x, int y, int w, int h, int bw)
{
	XWindowChanges wc;

	c->oldx = c->x; c->x = wc.x = x;
	c->oldy = c->y; c->y = wc.y = y;
	c->oldw = c->w; c->w = wc.width = w;
	c->oldh = c->h; c->h = wc.height = h;
	c->oldbw = c->bw; c->bw = wc.border_width = bw;
	XConfigureWindow(dpy, c->win, CWX|CWY|CWWidth|CWHeight|CWBorderWidth, &wc);
	configure(c);
	XSync(dpy, False);
}

void
resizemouse(const Arg *arg)
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

	XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w + c->bw - 1, c->h + c->bw - 1);

	const unsigned int snap_distance = settings_get_snap_distance();
	const int ocx = c->x;
	const int ocy = c->y;

	Time lasttime = 0;

	XEvent ev;

	do {
		XMaskEvent(dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);

		switch (ev.type) {
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			if ((ev.xmotion.time - lasttime) <= (1000 / 60)) continue;

			lasttime = ev.xmotion.time;

			const int nw = MAX(ev.xmotion.x - ocx - 2 * c->bw + 1, 1);
			const int nh = MAX(ev.xmotion.y - ocy - 2 * c->bw + 1, 1);

			if (c->mon->wx + nw >= selmon->wx &&
				c->mon->wx + nw <= selmon->wx + selmon->ww &&
				c->mon->wy + nh >= selmon->wy &&
				c->mon->wy + nh <= selmon->wy + selmon->wh)
			{
				if (!c->isfloating &&
					(selmon->lt[selmon->sellt]->arrange == NULL ||
						abs(nw - c->w) > snap_distance ||
						abs(nh - c->h) > snap_distance))
				{
					togglefloating(NULL);
				}
			}

			if (!selmon->lt[selmon->sellt]->arrange || c->isfloating) {
				resize(c, c->x, c->y, nw, nh, c->bw, 1);
			}

			break;
		}
	} while (ev.type != ButtonRelease);

	XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w + c->bw - 1, c->h + c->bw - 1);
	XUngrabPointer(dpy, CurrentTime);

	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));

	Monitor *const m = recttomon(c->x, c->y, c->w, c->h);

	if (m != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
}

void
restack(Monitor *m)
{
	Client *c;
	XEvent ev;
	XWindowChanges wc;

	drawbar(m);
	if (!m->sel)
		return;
	if (m->sel->isfloating || !m->lt[m->sellt]->arrange)
		XRaiseWindow(dpy, m->sel->win);
	if (m->lt[m->sellt]->arrange) {
		wc.stack_mode = Below;
		wc.sibling = m->barwin;
		for (c = m->stack; c; c = c->snext)
			if (!c->isfloating && ISVISIBLE(c)) {
				XConfigureWindow(dpy, c->win, CWSibling|CWStackMode, &wc);
				wc.sibling = c->win;
			}
	}
	XSync(dpy, False);
	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}

void
run(void)
{
	XEvent ev;
	/* main event loop */
	XSync(dpy, False);
	while (running && !XNextEvent(dpy, &ev))
		if (handler[ev.type])
			handler[ev.type](&ev); /* call handler */
}

void
scan(void)
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

void
sendmon(Client *c, Monitor *m)
{
	if (c->mon == m)
		return;
	unfocus(c, 1);
	detach(c);
	detachstack(c);
	c->mon = m;
	c->tags = m->tagset[m->seltags]; /* assign tags of target monitor */
	attach(c);
	attachstack(c);
	focus(NULL);
	arrange(NULL);
}

void
setclientstate(Client *c, long state)
{
	long data[] = { state, None };

	XChangeProperty(dpy, c->win, atoms->wmatom[WMState], atoms->wmatom[WMState], 32,
		PropModeReplace, (unsigned char *)data, 2);
}

int
sendevent(Window w, Atom proto, int mask, long d0, long d1, long d2, long d3, long d4)
{
	int n;
	Atom *protocols, mt;
	int exists = 0;
	XEvent ev;

	if (proto == atoms->wmatom[WMTakeFocus] || proto == atoms->wmatom[WMDelete]) {
		mt = atoms->wmatom[WMProtocols];
		if (XGetWMProtocols(dpy, w, &protocols, &n)) {
			while (!exists && n--)
				exists = protocols[n] == proto;
			XFree(protocols);
		}
	}
	else {
		exists = True;
		mt = proto;
	}
	if (exists) {
		ev.type = ClientMessage;
		ev.xclient.window = w;
		ev.xclient.message_type = mt;
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = d0;
		ev.xclient.data.l[1] = d1;
		ev.xclient.data.l[2] = d2;
		ev.xclient.data.l[3] = d3;
		ev.xclient.data.l[4] = d4;
		XSendEvent(dpy, w, False, mask, &ev);
	}
	return exists;
}

void
setfocus(Client *c)
{
	if (!c->neverfocus) {
		XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
		XChangeProperty(dpy, root, atoms->netatom[NetActiveWindow],
			XA_WINDOW, 32, PropModeReplace,
			(unsigned char *) &(c->win), 1);
	}
	sendevent(c->win, atoms->wmatom[WMTakeFocus], NoEventMask, atoms->wmatom[WMTakeFocus], CurrentTime, 0, 0, 0);
}

void
setfullscreen(Client *c, int fullscreen)
{
	if (fullscreen && !c->isfullscreen) {
		XChangeProperty(dpy, c->win, atoms->netatom[NetWMState], XA_ATOM, 32,
			PropModeReplace, (unsigned char*)&atoms->netatom[NetWMFullscreen], 1);
		c->isfullscreen = 1;
		// We have to rearrange because borders and gaps may have changed.
		arrange(c->mon);
	} else if (!fullscreen && c->isfullscreen){
		XChangeProperty(dpy, c->win, atoms->netatom[NetWMState], XA_ATOM, 32,
			PropModeReplace, (unsigned char*)0, 0);
		c->isfullscreen = 0;
		// We have to rearrange because borders and gaps may have changed.
		arrange(c->mon);
	}
}

void
setlayout(const Arg *arg)
{
	if (!arg || !arg->v || arg->v != selmon->lt[selmon->sellt]) {
		selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag] ^= 1;
	}

	if (arg && arg->v) {
		const Layout *const new_layout = arg->v;
		selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt] = new_layout;
		selmon->lt[selmon->sellt] = new_layout;
	}

	unsigned int visible_clients = 0;
	for (const Client *client = selmon->clients; client; client = client->next) {
		if (ISVISIBLE(client)) ++visible_clients;
	}

	layouts_symbol_func(
		selmon->lt[selmon->sellt]->symbol_func,
		selmon->ltsymbol,
		sizeof(selmon->ltsymbol),
		selmon->nmaster,
		visible_clients
	);

	if (selmon->sel) {
		arrange(selmon);
	} else {
		drawbar(selmon);
	}
}

/* arg > 1.0 will set mfact absolutely */
void
setmfact(const Arg *arg)
{
	float f;

	if (!arg || !selmon->lt[selmon->sellt]->arrange)
		return;
	f = arg->f < 1.0 ? arg->f + selmon->mfact : arg->f - 1.0;
	if (f < 0.05 || f > 0.95)
		return;
	selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag] = f;
	arrange(selmon);
}

void
setup(void)
{
	int i;
	XSetWindowAttributes wa;

	/* clean up any zombies immediately */
	sigchld(0);

	/* init screen */
	screen = DefaultScreen(dpy);
	sw = DisplayWidth(dpy, screen);
	sh = DisplayHeight(dpy, screen);
	root = RootWindow(dpy, screen);
	drw = drw_create(dpy, screen, root, sw, sh);
	if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
		die("no fonts could be loaded.");
	lrpad = drw->fonts->h;
	bh = drw->fonts->h + 2;
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
	for (i = 0; i < LENGTH(colors); i++)
		scheme[i] = drw_scm_create(drw, colors[i], 3);
	/* init system tray */
	updatesystray();
	/* init bars */
	updatebars();
	updatestatus();

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
}


void
seturgent(Client *c, int urg)
{
	XWMHints *wmh;

	c->isurgent = urg;
	if (!(wmh = XGetWMHints(dpy, c->win)))
		return;
	wmh->flags = urg ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
	XSetWMHints(dpy, c->win, wmh);
	XFree(wmh);
}

void
showhide(Client *c)
{
	if (!c)
		return;
	if (ISVISIBLE(c)) {
		/* show clients top down */
		XMoveWindow(dpy, c->win, c->x, c->y);
		if (!c->mon->lt[c->mon->sellt]->arrange || c->isfloating)
			resize(c, c->x, c->y, c->w, c->h, c->bw, 0);
		showhide(c->snext);
	} else {
		/* hide clients bottom up */
		showhide(c->snext);
		XMoveWindow(dpy, c->win, WIDTH(c) * -2, c->y);
	}
}

void
sigchld(int unused)
{
	if (signal(SIGCHLD, sigchld) == SIG_ERR)
		die("can't install SIGCHLD handler:");
	while (0 < waitpid(-1, NULL, WNOHANG));
}

void
spawn(const Arg *arg)
{
	const char *const command_name = arg->v;

	spawn_command(command_name, spawn_callback, selmon->num);
}

void
spawn_callback()
{
	if (dpy) {
		close(ConnectionNumber(dpy));
	}
}

void
tag(const Arg *arg)
{
	if (selmon->sel && arg->ui & TAGS_MASK) {
		selmon->sel->tags = arg->ui & TAGS_MASK;
		focus(NULL);
		arrange(selmon);
	}
}

void
tagmon(const Arg *arg)
{
	if (!selmon->sel || !mons->next)
		return;
	sendmon(selmon->sel, dirtomon(arg->i));
}

void
togglebar(const Arg *arg)
{
	selmon->showbar = selmon->pertag->showbars[selmon->pertag->curtag] = !selmon->showbar;

	updatebarpos(selmon);
	resizebarwin(selmon);
	if (showsystray) {
		XWindowChanges wc;
		if (!selmon->showbar)
			wc.y = -bh;
		else if (selmon->showbar) {
			wc.y = 0;
			if (!selmon->topbar)
				wc.y = selmon->mh - bh;
		}
		XConfigureWindow(dpy, systray->win, CWY, &wc);
	}
	arrange(selmon);
}

void
togglefloating(const Arg *arg)
{
	if (!selmon->sel) return;

	selmon->sel->isfloating = !selmon->sel->isfloating || selmon->sel->isfixed;

	const int border_width = settings_get_border_width();

	if (selmon->sel->isfloating) {
		resize(
			selmon->sel,
			selmon->sel->x,
			selmon->sel->y,
			selmon->sel->w - 2 * (border_width - selmon->sel->bw),
			selmon->sel->h - 2 * (border_width - selmon->sel->bw),
			border_width,
			0
		);
	}

	arrange(selmon);
}

void
toggletag(const Arg *arg)
{
	unsigned int newtags;

	if (!selmon->sel)
		return;
	newtags = selmon->sel->tags ^ (arg->ui & TAGS_MASK);
	if (newtags) {
		selmon->sel->tags = newtags;
		focus(NULL);
		arrange(selmon);
	}
}

void
toggleview(const Arg *arg)
{
	unsigned int newtagset = selmon->tagset[selmon->seltags] ^ (arg->ui & TAGS_MASK);
	int i;

	if (newtagset) {
		selmon->tagset[selmon->seltags] = newtagset;

		if (newtagset == ~0) {
			selmon->pertag->prevtag = selmon->pertag->curtag;
			selmon->pertag->curtag = 0;
		}

		/* test if the user did not select the same tag */
		if (!(newtagset & 1 << (selmon->pertag->curtag - 1))) {
			selmon->pertag->prevtag = selmon->pertag->curtag;
			for (i = 0; !(newtagset & 1 << i); i++) ;
			selmon->pertag->curtag = i + 1;
		}

		/* apply settings for this view */
		selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag];
		selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
		selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
		selmon->lt[selmon->sellt] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
		selmon->lt[selmon->sellt^1] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt^1];

		if (selmon->showbar != selmon->pertag->showbars[selmon->pertag->curtag] &&
			settings_get_show_bar_per_tag())
		{
			togglebar(NULL);
		}

		focus(NULL);
		arrange(selmon);
	}
}

void
unfocus(Client *c, int setfocus)
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

void
unmanage(Client *c, int destroyed)
{
	Monitor *m = c->mon;
	XWindowChanges wc;

	if (c->swallowing) {
		unswallow(c);
		return;
	}

	Client *s = swallowingclient(c->win);
	if (s) {
		free(s->swallowing);
		s->swallowing = NULL;
		arrange(m);
		focus(NULL);
		return;
	}

	detach(c);
	detachstack(c);
	if (!destroyed) {
		wc.border_width = c->oldbw;
		XGrabServer(dpy); /* avoid race conditions */
		XSetErrorHandler(xerrordummy);
		XConfigureWindow(dpy, c->win, CWBorderWidth, &wc); /* restore border */
		XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
		setclientstate(c, WithdrawnState);
		XSync(dpy, False);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
	free(c);

	if (!s) {
		arrange(m);
		focus(NULL);
		updateclientlist();
	}
}

void
updatebars(void)
{
	unsigned int w;
	Monitor *m;
	XSetWindowAttributes wa = {
		.override_redirect = True,
		.background_pixmap = ParentRelative,
		.event_mask = ButtonPressMask|ExposureMask
	};
	XClassHint ch = {"polytreewm", "polytreewm"};
	for (m = mons; m; m = m->next) {
		if (m->barwin)
			continue;
		w = m->ww;
		if (showsystray && m == systraytomon(m))
			w -= getsystraywidth();
		m->barwin = XCreateWindow(dpy, root, m->wx, m->by, w, bh, 0, DefaultDepth(dpy, screen),
				CopyFromParent, DefaultVisual(dpy, screen),
				CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
		XDefineCursor(dpy, m->barwin, cursor[CurNormal]->cursor);
		if (showsystray && m == systraytomon(m))
			XMapRaised(dpy, systray->win);
		XMapRaised(dpy, m->barwin);
		XSetClassHint(dpy, m->barwin, &ch);
	}
}

void
updatebarpos(Monitor *m)
{
	m->wy = m->my;
	m->wh = m->mh;
	if (m->showbar) {
		m->wh -= bh;
		m->by = m->topbar ? m->wy : m->wy + m->wh;
		m->wy = m->topbar ? m->wy + bh : m->wy;
	} else
		m->by = -bh;
}

void
updateclientlist()
{
	Client *c;
	Monitor *m;

	XDeleteProperty(dpy, root, atoms->netatom[NetClientList]);
	for (m = mons; m; m = m->next)
		for (c = m->clients; c; c = c->next)
			XChangeProperty(dpy, root, atoms->netatom[NetClientList],
				XA_WINDOW, 32, PropModeAppend,
				(unsigned char *) &(c->win), 1);
}

int
updategeom(void)
{
	int dirty = 0;

#ifdef XINERAMA
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
				if (i >= n
				|| unique[i].x_org != m->mx || unique[i].y_org != m->my
				|| unique[i].width != m->mw || unique[i].height != m->mh)
				{
					dirty = 1;
					m->num = i;
					m->mx = m->wx = unique[i].x_org;
					m->my = m->wy = unique[i].y_org;
					m->mw = m->ww = unique[i].width;
					m->mh = m->wh = unique[i].height;
					updatebarpos(m);
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
#endif /* XINERAMA */
	{ /* default monitor setup */
		if (!mons)
			mons = createmon();
		if (mons->mw != sw || mons->mh != sh) {
			dirty = 1;
			mons->mw = mons->ww = sw;
			mons->mh = mons->wh = sh;
			updatebarpos(mons);
		}
	}
	if (dirty) {
		selmon = mons;
		selmon = wintomon(root);
	}
	return dirty;
}

void
updatenumlockmask(void)
{
	unsigned int i, j;
	XModifierKeymap *modmap;

	numlockmask = 0;
	modmap = XGetModifierMapping(dpy);
	for (i = 0; i < 8; i++)
		for (j = 0; j < modmap->max_keypermod; j++)
			if (modmap->modifiermap[i * modmap->max_keypermod + j]
				== XKeysymToKeycode(dpy, XK_Num_Lock))
				numlockmask = (1 << i);
	XFreeModifiermap(modmap);
}

void
updatesizehints(Client *c)
{
	long msize;
	XSizeHints size;

	if (!XGetWMNormalHints(dpy, c->win, &size, &msize))
		/* size is uninitialized, ensure that size.flags aren't used */
		size.flags = PSize;
	if (size.flags & PBaseSize) {
		c->basew = size.base_width;
		c->baseh = size.base_height;
	} else if (size.flags & PMinSize) {
		c->basew = size.min_width;
		c->baseh = size.min_height;
	} else
		c->basew = c->baseh = 0;
	if (size.flags & PResizeInc) {
		c->incw = size.width_inc;
		c->inch = size.height_inc;
	} else
		c->incw = c->inch = 0;
	if (size.flags & PMaxSize) {
		c->maxw = size.max_width;
		c->maxh = size.max_height;
	} else
		c->maxw = c->maxh = 0;
	if (size.flags & PMinSize) {
		c->minw = size.min_width;
		c->minh = size.min_height;
	} else if (size.flags & PBaseSize) {
		c->minw = size.base_width;
		c->minh = size.base_height;
	} else
		c->minw = c->minh = 0;
	if (size.flags & PAspect) {
		c->mina = (float)size.min_aspect.y / size.min_aspect.x;
		c->maxa = (float)size.max_aspect.x / size.max_aspect.y;
	} else
		c->maxa = c->mina = 0.0;
	c->isfixed = (c->maxw && c->maxh && c->maxw == c->minw && c->maxh == c->minh);
}

void
updatestatus(void)
{
	if (!gettextprop(root, XA_WM_NAME, stext, sizeof(stext))) {
		strcpy(stext, "polytreewm-"VERSION);
	}

	for (Monitor *m = mons; m; m = m->next) {
		drawbar(m);
	}

	updatesystray();
}

void
updatetitle(Client *c)
{
	if (!gettextprop(c->win, atoms->netatom[NetWMName], c->name, sizeof c->name))
		gettextprop(c->win, XA_WM_NAME, c->name, sizeof c->name);
	if (c->name[0] == '\0') /* hack to mark broken clients */
		strcpy(c->name, broken);
}

void
updatewindowtype(Client *c)
{
	Atom state = getatomprop(c, atoms->netatom[NetWMState]);
	Atom wtype = getatomprop(c, atoms->netatom[NetWMWindowType]);

	if (state == atoms->netatom[NetWMFullscreen])
		setfullscreen(c, 1);
	if (wtype == atoms->netatom[NetWMWindowTypeDialog])
		c->isfloating = 1;
}

void
updatewmhints(Client *c)
{
	XWMHints *wmh;

	if ((wmh = XGetWMHints(dpy, c->win))) {
		if (c == selmon->sel && wmh->flags & XUrgencyHint) {
			wmh->flags &= ~XUrgencyHint;
			XSetWMHints(dpy, c->win, wmh);
		} else
			c->isurgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
		if (wmh->flags & InputHint)
			c->neverfocus = !wmh->input;
		else
			c->neverfocus = 0;
		XFree(wmh);
	}
}

void
view(const Arg *arg)
{
	const unsigned int old_tagset = selmon->tagset[selmon->seltags] & TAGS_MASK;
	const unsigned int new_tagset = arg->ui & TAGS_MASK;

	if (new_tagset == old_tagset) return;

	selmon->seltags ^= 1; /* toggle sel tagset */

	if (new_tagset == 0) {
		unsigned int tmptag = selmon->pertag->prevtag;
		selmon->pertag->prevtag = selmon->pertag->curtag;
		selmon->pertag->curtag = tmptag;
	} else {
		selmon->tagset[selmon->seltags] = new_tagset;
		selmon->pertag->prevtag = selmon->pertag->curtag;

		if (arg->ui == ~0) {
			selmon->pertag->curtag = 0;
		} else {
			int i = 0;
			while (!(new_tagset & 1 << i)) ++i;
			selmon->pertag->curtag = i + 1;
		}
	}

	selmon->nmaster               = selmon->pertag->nmasters[selmon->pertag->curtag];
	selmon->mfact                 = selmon->pertag->mfacts[selmon->pertag->curtag];
	selmon->sellt                 = selmon->pertag->sellts[selmon->pertag->curtag];
	selmon->lt[selmon->sellt]     = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
	selmon->lt[selmon->sellt ^ 1] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt ^ 1];

	if (selmon->showbar != selmon->pertag->showbars[selmon->pertag->curtag] &&
		settings_get_show_bar_per_tag())
	{
		togglebar(NULL);
	}

	focus(NULL);
	arrange(selmon);
}

void
viewrel(const Arg *arg)
{
	const int shift = arg->i;

	if (shift == 0) return;

	const unsigned int old_tagset = selmon->tagset[selmon->seltags] & TAGS_MASK;
	const unsigned int new_tagset = (shift > 0 ? (old_tagset << shift) : (old_tagset >> (-shift))) & TAGS_MASK;

	if (new_tagset == old_tagset) return;

	if (new_tagset == 0) {
		unsigned int tmptag = selmon->pertag->prevtag;
		selmon->pertag->prevtag = selmon->pertag->curtag;
		selmon->pertag->curtag = tmptag;
	} else {
		selmon->tagset[selmon->seltags] = new_tagset;
		selmon->pertag->prevtag = selmon->pertag->curtag;

		if (new_tagset == (~0 & TAGS_MASK)) {
			selmon->pertag->curtag = 0;
		} else {
			int i = 0;
			while (!(new_tagset & 1 << i)) ++i;
			selmon->pertag->curtag = i + 1;
		}
	}

	selmon->nmaster               = selmon->pertag->nmasters[selmon->pertag->curtag];
	selmon->mfact                 = selmon->pertag->mfacts[selmon->pertag->curtag];
	selmon->sellt                 = selmon->pertag->sellts[selmon->pertag->curtag];
	selmon->lt[selmon->sellt]     = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
	selmon->lt[selmon->sellt ^ 1] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt ^ 1];

	if (selmon->showbar != selmon->pertag->showbars[selmon->pertag->curtag] &&
		settings_get_show_bar_per_tag())
	{
		togglebar(NULL);
	}

	focus(NULL);
	arrange(selmon);
}

Client *
wintoclient(Window w)
{
	Client *c;
	Monitor *m;

	for (m = mons; m; m = m->next)
		for (c = m->clients; c; c = c->next)
			if (c->win == w)
				return c;
	return NULL;
}

Monitor *
wintomon(Window w)
{
	int x, y;
	Client *c;
	Monitor *m;

	if (w == root && getrootptr(&x, &y))
		return recttomon(x, y, 1, 1);
	for (m = mons; m; m = m->next)
		if (w == m->barwin)
			return m;
	if ((c = wintoclient(w)))
		return c->mon;
	return selmon;
}

/* There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's). Other types of errors call Xlibs
 * default error handler, which may call exit. */
int
xerror(Display *dpy, XErrorEvent *ee)
{
	if (ee->error_code == BadWindow
	|| (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
	|| (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
	|| (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
	|| (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
	|| (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
	|| (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
		return 0;
	fprintf(stderr, "polytreewm: fatal error: request code=%d, error code=%d\n",
		ee->request_code, ee->error_code);
	return xerrorxlib(dpy, ee); /* may call exit */
}

int
xerrordummy(Display *dpy, XErrorEvent *ee)
{
	return 0;
}

/* Startup Error handler to check if another window manager
 * is already running. */
int
xerrorstart(Display *dpy, XErrorEvent *ee)
{
	die("polytreewm: another window manager is already running");
	return -1;
}

void
zoom(const Arg *arg)
{
	Client *c = selmon->sel;

	if (!selmon->lt[selmon->sellt]->arrange
	|| (selmon->sel && selmon->sel->isfloating))
		return;
	if (c == nexttiled(selmon->clients))
		if (!c || !(c = nexttiled(c->next)))
			return;
	pop(c);
}
