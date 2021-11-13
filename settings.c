#include "settings.h"

static bool focus_on_wheel = true;

bool settings_get_focus_on_wheel()
{
	return focus_on_wheel;
}

void settings_set_focus_on_wheel(const bool new_focus_on_wheel)
{
	focus_on_wheel = new_focus_on_wheel;
}
