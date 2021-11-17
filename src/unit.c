#include "unit.h"

#include "settings.h"

#include <stdlib.h>
#include <string.h>

Unit unit_new()
{
	Unit unit = malloc(sizeof(struct Unit));
	memset(unit, 0, sizeof(struct Unit));
	unit->show_bar = settings_get_show_bar_by_default();
	return unit;
}

void unit_delete(const Unit unit)
{
	// TODO: maybe we should assert
	if (unit == NULL) return;

	free(unit);
}
