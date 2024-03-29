#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "unit.h"

#include <stdbool.h>

typedef unsigned char SettingsForSingleWindow;

#define SETTINGS_FOR_SINGLE_WINDOW_NEVER                0
#define SETTINGS_FOR_SINGLE_WINDOW_ALWAYS               1
#define SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN    2
#define SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN 3

bool settings_get_bar_on_top_by_default();
void settings_set_bar_on_top_by_default(bool new_bar_on_top_by_default);

void settings_set_border_for_single_window(SettingsForSingleWindow new_border_for_single_window);

int  settings_get_border_width();
void settings_set_border_width(int new_border_width);

int  settings_get_default_clients_in_master();
void settings_set_default_clients_in_master(int new_default_clients_in_master);

float settings_get_default_master_area_factor();
void  settings_set_default_master_area_factor(float new_default_master_area_factor);

void settings_set_gap_for_single_window(SettingsForSingleWindow new_gap_for_single_window);

bool settings_get_enable_swallowing();
void settings_set_enable_swallowing(bool new_enable_swallowing);

bool settings_get_focus_on_wheel();
void settings_set_focus_on_wheel(bool new_focus_on_wheel);

int  settings_get_gap_size();
void settings_set_gap_size(int new_gap_size);

UnitKind settings_get_master_area_factor_per_unit();
void     settings_set_master_area_factor_per_unit(UnitKind new_master_area_factor_per_unit);

int  settings_get_max_clients_in_master();
void settings_set_max_clients_in_master(int new_max_clients_in_master);

bool settings_get_respect_resize_hints_in_floating_layout();
void settings_set_respect_resize_hints_in_floating_layout(bool new_respect_resize_hints_in_floating_layout);

bool settings_get_show_bar_by_default();
void settings_set_show_bar_by_default(bool new_show_bar_by_default);

UnitKind settings_get_show_bar_per_unit();
void     settings_set_show_bar_per_unit(UnitKind new_show_bar_per_unit);

unsigned int settings_get_snap_distance();
void         settings_set_snap_distance(unsigned int new_snap_distance);

bool settings_get_swallow_floating();
void settings_set_swallow_floating(bool new_swallow_floating);

#endif // _SETTINGS_H
