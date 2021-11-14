#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H

#include <stddef.h>

#define DATASTRUCT_DELETE(datastruct) { \
	datastruct_delete(datastruct);      \
	datastruct = NULL;                  \
}

typedef struct Datastruct *Datastruct;
typedef struct DatastructItem *DatastructItem;

Datastruct datastruct_new();
void datastruct_delete(Datastruct datastruct);

void *datastruct_item_value(DatastructItem item);

DatastructItem datastruct_push(Datastruct datastruct, const void *new_value);

void datastruct_remove(Datastruct datastruct, DatastructItem old_item);

DatastructItem datastruct_insert_after(
	DatastructItem after_item,
	const void *new_value
);

#endif // _DATASTRUCT_H
