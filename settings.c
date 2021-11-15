#include "settings.h"

static int border_width = 2;
static int default_clients_in_master = 1;
static bool enable_gap_for_single_window = true;
static bool focus_on_wheel = true;
static int gap_size = 10;
static int max_clients_in_master = 0; // 0 for no maximum
static bool respect_resize_hints_in_floating_layout = false;
static unsigned int snap_distance = 32;

int  settings_get_border_width()
{
	return border_width;
}

void settings_set_border_width(const int new_border_width)
{
	border_width = new_border_width;
	// TODO: notify WM to rearrange clients
}

int settings_get_default_clients_in_master()
{
	return default_clients_in_master >= 1 ? default_clients_in_master : 1;
}

void settings_set_default_clients_in_master(const int new_default_clients_in_master)
{
	default_clients_in_master = new_default_clients_in_master;
}

bool settings_get_enable_gap_for_single_window()
{
	return enable_gap_for_single_window;
}

void settings_set_enable_gap_for_single_window(const bool new_enable_gap_for_single_window)
{
	enable_gap_for_single_window = new_enable_gap_for_single_window;
	// TODO: notify WM to rearrange clients
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
	gap_size = new_gap_size >= 0 ? new_gap_size : 0;
	// TODO: notify WM to rearrange clients
}

int settings_get_max_clients_in_master()
{
	return max_clients_in_master > 0 ? max_clients_in_master : 0;
}

void settings_set_max_clients_in_master(const int new_max_clients_in_master)
{
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

unsigned int settings_get_snap_distance()
{
	return snap_distance;
}

void settings_set_snap_distance(const unsigned int new_snap_distance)
{
	snap_distance = new_snap_distance;
}
