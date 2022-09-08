#include "settings.h"

#include "constraints.h"

static UnitKind master_area_factor_per_unit = UNIT_MONITOR;
static UnitKind show_bar_per_unit = UNIT_MONITOR;

UnitKind settings_get_master_area_factor_per_unit()
{
	return master_area_factor_per_unit;
}

void settings_set_master_area_factor_per_unit(const UnitKind new_master_area_factor_per_unit)
{
	master_area_factor_per_unit = new_master_area_factor_per_unit;
	// TODO: notify WM to rearrange clients
}

UnitKind settings_get_show_bar_per_unit()
{
	return show_bar_per_unit;
}

void settings_set_show_bar_per_unit(const UnitKind new_show_bar_per_unit)
{
	show_bar_per_unit = new_show_bar_per_unit;
	// TODO: notify WM to rearrange clients
}
