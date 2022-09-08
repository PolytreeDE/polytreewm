#include "settings.h"

#include "constraints.h"

static SettingsForSingleWindow border_for_single_window = SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN;
static SettingsForSingleWindow gap_for_single_window = SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN;
static UnitKind master_area_factor_per_unit = UNIT_MONITOR;
static UnitKind show_bar_per_unit = UNIT_MONITOR;
static unsigned int snap_distance = 32;
static bool swallow_floating = false;

SettingsForSingleWindow settings_get_border_for_single_window()
{
	return border_for_single_window;
}

void settings_set_border_for_single_window(const SettingsForSingleWindow new_border_for_single_window)
{
	border_for_single_window = new_border_for_single_window;
	// TODO: notify WM to rearrange clients
}

SettingsForSingleWindow settings_get_gap_for_single_window()
{
	return gap_for_single_window;
}

void settings_set_gap_for_single_window(const SettingsForSingleWindow new_gap_for_single_window)
{
	gap_for_single_window = new_gap_for_single_window;
	// TODO: notify WM to rearrange clients
}

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

unsigned int settings_get_snap_distance()
{
	return snap_distance;
}

void settings_set_snap_distance(unsigned int new_snap_distance)
{
	snap_distance = constraints_snap_distance(new_snap_distance);
}

bool settings_get_swallow_floating()
{
	return swallow_floating;
}

void settings_set_swallow_floating(const bool new_swallow_floating)
{
	swallow_floating = new_swallow_floating;
}
