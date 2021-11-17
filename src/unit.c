#include "unit.h"

#include "settings.h"

#include <stdlib.h>
#include <string.h>

struct Unit {
	UnitKind kind;
	struct Unit *parent;
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
	const UnitKind show_bar_per_unit = settings_get_show_bar_per_unit();

	if (unit->kind == show_bar_per_unit) {
		return unit->show_bar;
	} else if (unit->kind > show_bar_per_unit) {
		return unit_get_show_bar(unit->parent);
	} else {
		// TODO: maybe we should assert here
		return settings_get_show_bar_by_default();
	}
}

bool unit_toggle_show_bar(const Unit unit)
{
	const UnitKind show_bar_per_unit = settings_get_show_bar_per_unit();

	if (unit->kind == show_bar_per_unit) {
		return unit->show_bar = !unit->show_bar;
	} else if (unit->kind > show_bar_per_unit) {
		return unit_toggle_show_bar(unit->parent);
	} else {
		// TODO: maybe we should assert here
		return settings_get_show_bar_by_default();
	}
}
