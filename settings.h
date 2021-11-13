#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdbool.h>

int  settings_get_default_clients_in_master();
void settings_set_default_clients_in_master(int new_default_clients_in_master);

bool settings_get_focus_on_wheel();
void settings_set_focus_on_wheel(bool new_focus_on_wheel);

int  settings_get_max_clients_in_master();
void settings_set_max_clients_in_master(int new_max_clients_in_master);

bool settings_get_respect_resize_hints_in_floating_layout();
void settings_set_respect_resize_hints_in_floating_layout(bool new_respect_resize_hints_in_floating_layout);

unsigned int settings_get_snap_distance();
void         settings_set_snap_distance(unsigned int new_snap_distance);

#endif // _SETTINGS_H
