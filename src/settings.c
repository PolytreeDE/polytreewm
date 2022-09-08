#include "settings.h"

#include "constraints.h"

static UnitKind show_bar_per_unit = UNIT_MONITOR;

UnitKind settings_get_show_bar_per_unit()
{
	return show_bar_per_unit;
}

void settings_set_show_bar_per_unit(const UnitKind new_show_bar_per_unit)
{
	show_bar_per_unit = new_show_bar_per_unit;
	// TODO: notify WM to rearrange clients
}
