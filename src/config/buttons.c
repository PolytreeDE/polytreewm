#include "buttons.h"

#include "../dwm.h"

#include <X11/keysym.h>

#define MODKEY Mod4Mask

const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
};

size_t buttons_count()
{
	return sizeof(buttons) / sizeof(buttons[0]);
}
