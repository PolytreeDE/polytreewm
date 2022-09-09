#ifndef _DWM_INTERACTION_H
#define _DWM_INTERACTION_H

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

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

static void configborder(const Arg *arg);
static void configgap(const Arg *arg);
static void dorestart(const Arg *arg);
static void focusmon(const Arg *arg);
static void focusstack(const Arg *arg);
static void incnmaster(const Arg *arg);
static void killclient(const Arg *arg);
static void movemouse(const Arg *arg);
static void movestack(const Arg *arg);
static void quit(const Arg *arg);
static void resetnmaster(const Arg *arg);
static void resizemouse(const Arg *arg);
static void setlayout(const Arg *arg);
static void setmfact(const Arg *arg);
static void spawn(const Arg *arg);
static void tagmon(const Arg *arg);
static void togglefloating(const Arg *arg);
static void zoom(const Arg *arg);

static void spawn_callback();

#define MODKEY Mod4Mask

static Key keys[] = {
	// WM
	{ MODKEY|ControlMask|ShiftMask, XK_q,            quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_r,            dorestart,      {0} },
	// Monitor
	{ MODKEY,                       XK_bracketleft,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_bracketright, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_bracketleft,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_bracketright, tagmon,         {.i = +1 } },
	// Layout
	{ MODKEY,                       XK_m,            setlayout,      {.v = &layouts[0]} }, // Monocle
	{ MODKEY,                       XK_f,            setlayout,      {.v = &layouts[1]} }, // Floating
	{ MODKEY,                       XK_t,            setlayout,      {.v = &layouts[2]} }, // Tile
	{ MODKEY|ShiftMask,             XK_t,            setlayout,      {.v = &layouts[3]} }, // Horizontile
	{ MODKEY,                       XK_u,            setlayout,      {.v = &layouts[4]} }, // Centeredmaster
	{ MODKEY,                       XK_space,        setlayout,      {0} },
	{ MODKEY,                       XK_i,            incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,            resetnmaster,   {.i = 1 } },
	{ MODKEY,                       XK_d,            incnmaster,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_d,            resetnmaster,   {.i = 0 } },
	// Stack
	{ MODKEY,                       XK_j,            focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,            focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,            movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,            movestack,      {.i = -1 } },
	{ MODKEY,                       XK_Return,       zoom,           {0} },
	// Window
	{ MODKEY|ShiftMask,             XK_x,            killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_space,        togglefloating, {0} },
	// Appearance
	{ MODKEY,                       XK_h,            setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,            setmfact,       {.f = +0.05} },
	{ MODKEY|Mod1Mask,              XK_b,            configborder,   {.i = -1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_b,            configborder,   {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_g,            configgap,      {.i = -1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_g,            configgap,      {.i = +1 } },
	// Starting applications
	{ MODKEY,                       XK_z,            spawn,          {.v = "lock" } },
	{ MODKEY,                       XK_slash,        spawn,          {.v = "menu" } },
	{ MODKEY|ShiftMask,             XK_slash,        spawn,          {.v = "term" } },
	{ MODKEY|ShiftMask,             XK_f,            spawn,          {.v = "firefox" } },
};

// click can be ClkClientWin, or ClkRootWin
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
};

#endif // _DWM_INTERACTION_H
