#include "settings.h"

#include "constraints.h"

static SettingsForSingleWindow border_for_single_window = SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN;
static int border_width = 2;
static int default_clients_in_master = 1;
static float default_master_area_factor = 0.6;
static SettingsForSingleWindow gap_for_single_window = SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN;
static bool enable_swallowing = true;
static bool focus_on_wheel = true;
static int gap_size = 10;
static UnitKind master_area_factor_per_unit = UNIT_MONITOR;
static int max_clients_in_master = 0; // 0 for no maximum
static bool respect_resize_hints_in_floating_layout = false;
static bool show_bar_by_default = true;
static UnitKind show_bar_per_unit = UNIT_MONITOR;
static unsigned int snap_distance = 32;
static bool status_on_all_monitors = false;
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

int settings_get_border_width()
{
	return border_width;
}

void settings_set_border_width(const int new_border_width)
{
	border_width = constraints_border_width(new_border_width);
	// TODO: notify WM to rearrange clients
}

int settings_get_default_clients_in_master()
{
	return default_clients_in_master;
}

void settings_set_default_clients_in_master(const int new_default_clients_in_master)
{
	default_clients_in_master = constraints_default_clients_in_master(new_default_clients_in_master);
}

float settings_get_default_master_area_factor()
{
	return default_master_area_factor;
}

void settings_set_default_master_area_factor(const float new_default_master_area_factor)
{
	default_master_area_factor = constraints_default_master_area_factor(new_default_master_area_factor);
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

bool settings_get_enable_swallowing()
{
	return enable_swallowing;
}

void settings_set_enable_swallowing(const bool new_enable_swallowing)
{
	enable_swallowing = new_enable_swallowing;
}


bool settings_get_focus_on_wheel()
{
	return focus_on_wheel;
}

void settings_set_focus_on_wheel(const bool new_focus_on_wheel)
{
	focus_on_wheel = new_focus_on_wheel;
}

int settings_get_gap_size()
{
	return gap_size;
}

void settings_set_gap_size(const int new_gap_size)
{
	gap_size = constraints_gap_size(new_gap_size);
	// TODO: notify WM to rearrange clients
}

float settings_get_master_area_factor_per_unit()
{
	return master_area_factor_per_unit;
}

void settings_set_master_area_factor_per_unit(const UnitKind new_master_area_factor_per_unit)
{
	master_area_factor_per_unit = new_master_area_factor_per_unit;
	// TODO: notify WM to rearrange clients
}

int settings_get_max_clients_in_master()
{
	return max_clients_in_master;
}

void settings_set_max_clients_in_master(const int new_max_clients_in_master)
{
	max_clients_in_master = constraints_max_clients_in_master(new_max_clients_in_master);
	// TODO: notify WM to rearrange clients
}

bool settings_get_respect_resize_hints_in_floating_layout()
{
	return respect_resize_hints_in_floating_layout;
}

void settings_set_respect_resize_hints_in_floating_layout(const bool new_respect_resize_hints_in_floating_layout)
{
	respect_resize_hints_in_floating_layout = new_respect_resize_hints_in_floating_layout;
	// TODO: notify WM to rearrange clients
}

bool settings_get_show_bar_by_default()
{
	return show_bar_by_default;
}

void settings_set_show_bar_by_default(const bool new_show_bar_by_default)
{
	show_bar_by_default = new_show_bar_by_default;
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

bool settings_get_status_on_all_monitors()
{
	return status_on_all_monitors;
}

void settings_set_status_on_all_monitors(const bool new_status_on_all_monitors)
{
	status_on_all_monitors = new_status_on_all_monitors;
	// TODO: notify WM to rearrange clients
}

bool settings_get_swallow_floating()
{
	return swallow_floating;
}

void settings_set_swallow_floating(const bool new_swallow_floating)
{
	swallow_floating = new_swallow_floating;
}
