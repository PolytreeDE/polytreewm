#include "settings.h"

static int default_clients_in_master = 1;
static bool focus_on_wheel = true;
static int max_clients_in_master = 0; // 0 for no maximum
static bool respect_resize_hints_in_floating_layout = false;
static unsigned int snap_distance = 32;

int settings_get_default_clients_in_master()
{
	return default_clients_in_master >= 1 ? default_clients_in_master : 1;
}

void settings_set_default_clients_in_master(const int new_default_clients_in_master)
{
	default_clients_in_master = new_default_clients_in_master;
}

bool settings_get_focus_on_wheel()
{
	return focus_on_wheel;
}

void settings_set_focus_on_wheel(const bool new_focus_on_wheel)
{
	focus_on_wheel = new_focus_on_wheel;
}

int settings_get_max_clients_in_master()
{
	return max_clients_in_master > 0 ? max_clients_in_master : 0;
}

void settings_set_max_clients_in_master(const int new_max_clients_in_master)
{
	max_clients_in_master = new_max_clients_in_master;
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
