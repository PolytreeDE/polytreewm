#include "settings.h"

#define MIN_BORDER_WIDTH 0
#define MAX_BORDER_WIDTH 10000
#define MIN_DEFAULT_CLIENTS_IN_MASTER 1
#define MAX_DEFAULT_CLIENTS_IN_MASTER 10000
#define MIN_GAP_SIZE 0
#define MAX_GAP_SIZE 10000
#define MIN_MAX_CLIENTS_IN_MASTER 1
#define MAX_MAX_CLIENTS_IN_MASTER 10000
#define MIN_SNAP_DISTANCE 1
#define MAX_SNAP_DISTANCE 10000

static SettingsForSingleWindow border_for_single_window = SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN;
static int border_width = 2;
static int default_clients_in_master = 1;
static SettingsForSingleWindow gap_for_single_window = SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN;
static bool enable_swallowing = true;
static bool focus_on_wheel = true;
static int gap_size = 10;
static int max_clients_in_master = 0; // 0 for no maximum
static bool respect_resize_hints_in_floating_layout = false;
static bool show_bar_by_default = true;
static bool show_bar_per_unit = UNIT_MONITOR;
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

void settings_set_border_width(int new_border_width)
{
	if (new_border_width < MIN_BORDER_WIDTH) new_border_width = MIN_BORDER_WIDTH;
	if (new_border_width > MAX_BORDER_WIDTH) new_border_width = MAX_BORDER_WIDTH;
	border_width = new_border_width;
	// TODO: notify WM to rearrange clients
}

int settings_get_default_clients_in_master()
{
	return default_clients_in_master;
}

void settings_set_default_clients_in_master(int new_default_clients_in_master)
{
	if (new_default_clients_in_master < MIN_DEFAULT_CLIENTS_IN_MASTER) new_default_clients_in_master = MIN_DEFAULT_CLIENTS_IN_MASTER;
	if (new_default_clients_in_master < MAX_DEFAULT_CLIENTS_IN_MASTER) new_default_clients_in_master = MAX_DEFAULT_CLIENTS_IN_MASTER;
	default_clients_in_master = new_default_clients_in_master;
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

void settings_set_gap_size(int new_gap_size)
{
	if (new_gap_size < MIN_GAP_SIZE) new_gap_size = MIN_GAP_SIZE;
	if (new_gap_size > MAX_GAP_SIZE) new_gap_size = MAX_GAP_SIZE;
	gap_size = new_gap_size;
	// TODO: notify WM to rearrange clients
}

int settings_get_max_clients_in_master()
{
	return max_clients_in_master;
}

void settings_set_max_clients_in_master(int new_max_clients_in_master)
{
	if (new_max_clients_in_master < MIN_MAX_CLIENTS_IN_MASTER) new_max_clients_in_master = MIN_MAX_CLIENTS_IN_MASTER;
	if (new_max_clients_in_master < MAX_MAX_CLIENTS_IN_MASTER) new_max_clients_in_master = MAX_MAX_CLIENTS_IN_MASTER;
	max_clients_in_master = new_max_clients_in_master;
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

void settings_set_show_bar_by_default(bool new_show_bar_by_default)
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
	if (new_snap_distance < MIN_SNAP_DISTANCE) new_snap_distance = MIN_SNAP_DISTANCE;
	if (new_snap_distance < MAX_SNAP_DISTANCE) new_snap_distance = MAX_SNAP_DISTANCE;
	snap_distance = new_snap_distance;
}

bool settings_get_status_on_all_monitors()
{
	return status_on_all_monitors;
}

void settings_set_status_on_all_monitors(bool new_status_on_all_monitors)
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
