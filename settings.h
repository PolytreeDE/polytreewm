#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdbool.h>

bool settings_get_focus_on_wheel();
void settings_set_focus_on_wheel(bool new_focus_on_wheel);

bool settings_get_respect_resize_hints_in_floating_layout();
void settings_set_respect_resize_hints_in_floating_layout(bool new_respect_resize_hints_in_floating_layout);

#endif // _SETTINGS_H
