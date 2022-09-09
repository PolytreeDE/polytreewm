#include "keys.h"

#include "../dwm.h"
#include "layouts.h"

#include <X11/keysym.h>

#define MODKEY Mod4Mask

const Key keys[] = {
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

size_t keys_count()
{
	return sizeof(keys) / sizeof(keys[0]);
}
