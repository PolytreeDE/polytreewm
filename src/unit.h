#ifndef _UNIT_H
#define _UNIT_H

#include <stdbool.h>
#include <stddef.h>

#define UNIT_DELETE(unit) { \
	unit_delete(unit);      \
	unit = NULL;            \
}

typedef struct Unit {
	bool show_bar;
} *Unit;

Unit unit_new();

void unit_delete(Unit unit);

#endif // _UNIT_H
