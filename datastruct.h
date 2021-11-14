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

DatastructItem datastruct_top(Datastruct datastruct);
DatastructItem datastruct_next(DatastructItem item);
DatastructItem datastruct_find_first(Datastruct datastruct, const void *value);
DatastructItem datastruct_find_last(Datastruct datastruct, const void *value);
void datastruct_remove(Datastruct datastruct, DatastructItem item);
DatastructItem datastruct_push(Datastruct datastruct, const void *value);
DatastructItem datastruct_insert(DatastructItem item, const void *value);

#endif // _DATASTRUCT_H
