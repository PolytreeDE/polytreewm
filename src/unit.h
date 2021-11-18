#ifndef _UNIT_H
#define _UNIT_H

#include <stdbool.h>
#include <stddef.h>

#define UNIT_DELETE(unit) { \
	unit_delete(unit);      \
	unit = NULL;            \
}

typedef enum {
	UNIT_GLOBAL  = 0,
	UNIT_MONITOR = 1,
	UNIT_TAG     = 2,
} UnitKind;

typedef struct Unit *Unit;

Unit unit_new(UnitKind kind, Unit parent);
void unit_delete(Unit unit);

bool unit_get_show_bar(Unit unit);
bool unit_toggle_show_bar(Unit unit);

float unit_get_master_area_factor(Unit unit);
float unit_inc_master_area_factor(Unit unit, float delta);

#endif // _UNIT_H
