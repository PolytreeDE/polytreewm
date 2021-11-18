#include "unit.h"

#include "constraints.h"
#include "settings.h"

#include <stdlib.h>
#include <string.h>

struct Unit {
	UnitKind kind;
	struct Unit *parent;
	bool show_bar;
	float master_area_factor;
};

Unit unit_new(const UnitKind kind, const Unit parent)
{
	Unit unit = malloc(sizeof(struct Unit));
	if (!unit) goto fail_without_unit;

	memset(unit, 0, sizeof(struct Unit));
	unit->kind = kind;
	unit->parent = parent;

	unit->show_bar           = settings_get_show_bar_by_default();
	unit->master_area_factor = settings_get_default_master_area_factor();

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

float unit_get_master_area_factor(const Unit unit)
{
	const UnitKind master_area_factor_per_unit =
		settings_get_master_area_factor_per_unit();

	if (unit->kind == master_area_factor_per_unit) {
		return unit->master_area_factor;
	} else if (unit->kind > master_area_factor_per_unit) {
		return unit_get_master_area_factor(unit->parent);
	} else {
		// TODO: maybe we should assert here
		return settings_get_default_master_area_factor();
	}
}

float unit_inc_master_area_factor(const Unit unit, const float delta)
{
	const UnitKind master_area_factor_per_unit =
		settings_get_master_area_factor_per_unit();

	if (unit->kind == master_area_factor_per_unit) {
		return unit->master_area_factor =
			constraints_master_area_factor(unit->master_area_factor + delta);
	} else if (unit->kind > master_area_factor_per_unit) {
		return unit_inc_master_area_factor(unit->parent, delta);
	} else {
		// TODO: maybe we should assert here
		return settings_get_default_master_area_factor();
	}
}
