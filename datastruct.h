#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H

#include <stddef.h>

#define DATASTRUCT_DELETE(datastruct) { \
	datastruct_delete(datastruct);      \
	datastruct = NULL;                  \
}

typedef struct Datastruct *Datastruct;

Datastruct datastruct_new();
void datastruct_delete(Datastruct datastruct);

void datastruct_push(Datastruct datastruct, const void *new_value);

void datastruct_insert_after_value(
	Datastruct datastruct,
	const void *after_value,
	const void *new_value
);

#endif // _DATASTRUCT_H
