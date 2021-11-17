/* See LICENSE file for copyright and license details. */

static const char wm_name[] = "PolytreeWM";

/* appearance */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
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

static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 1;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class,   instance, title,          tags mask, isfloating, isterminal, noswallow, monitor */
	{ "ptterm", NULL,     NULL,           0,         0,          1,          0,         -1      },
	{ NULL,     NULL,     "Event Tester", 0,         0,          0,          1,         -1      }, // xev
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */

static const Layout layouts[] = {
	/* symbol function,              arrange function */
	{ layouts_symbol_monocle,        monocle }, /* first entry is default */
	{ layouts_symbol_floating,       NULL },    /* no layout function means floating behavior */
	{ layouts_symbol_tile,           tile },
	{ layouts_symbol_horizontile,    horizontile },
	{ layouts_symbol_centeredmaster, centeredmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_n,      nametag,        {0} },
	{ MODKEY,                       XK_z,      spawn,          {.v = "lock" } },
	{ MODKEY,                       XK_slash,  spawn,          {.v = "menu" } },
	{ MODKEY|ShiftMask,             XK_slash,  spawn,          {.v = "term" } },
	{ MODKEY|ShiftMask,             XK_f,      spawn,          {.v = "firefox" } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      resetnmaster,   {.i = 1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_d,      resetnmaster,   {.i = 0 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,      viewrel,        {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_l,      viewrel,        {.i = +1 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_x,      killclient,     {0} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[0]} }, // Monocle
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} }, // Floating
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[2]} }, // Tile
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[3]} }, // Horizontile
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[4]} }, // Centeredmaster
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|Mod1Mask,              XK_b,      configborder,   {.i = -1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_b,      configborder,   {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_g,      configgap,      {.i = -1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_g,      configgap,      {.i = +1 } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
