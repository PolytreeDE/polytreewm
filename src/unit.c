#include "unit.h"

#include "settings.h"

#include <stdlib.h>
#include <string.h>

struct Unit {
	UnitKind kind;
	const struct Unit *parent;
	bool show_bar;
};

Unit unit_new(const UnitKind kind, const Unit parent)
{
	Unit unit = malloc(sizeof(struct Unit));
	if (!unit) goto fail_without_unit;

	memset(unit, 0, sizeof(struct Unit));
	unit->kind = kind;
	unit->parent = parent;
	unit->show_bar = settings_get_show_bar_by_default();

	if (unit->kind == UNIT_GLOBAL) {
		// TODO: maybe we should assert here
		if (unit->parent) goto fail_other;
	} else {
		if (!unit->parent || unit->kind != unit->parent->kind + 1) {
			// TODO: maybe we should assert here
			goto fail_other;
		}
	}

	return unit;

fail_other:
	free(unit);
fail_without_unit:
	return NULL;
}

void unit_delete(const Unit unit)
{
	// TODO: maybe we should assert
	if (unit == NULL) return;

	free(unit);
}

bool unit_get_show_bar(const Unit unit)
{
	return unit->show_bar;
}

bool unit_toggle_show_bar(const Unit unit)
{
	return unit->show_bar = !unit->show_bar;
}
