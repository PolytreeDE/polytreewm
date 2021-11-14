#include "datastruct.h"

#include <stdlib.h>

struct Datastruct {
	DatastructItem top;
};

struct DatastructItem {
	DatastructItem next;
	const void *value;
};

Datastruct datastruct_new()
{
	Datastruct datastruct = malloc(sizeof(struct Datastruct));
	if (datastruct == NULL) return NULL;
	datastruct->top = NULL;
	return datastruct;
}

void datastruct_delete(const Datastruct datastruct)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL) return;

	for (DatastructItem item = datastruct->top; item; item = item->next) free(item);
	free(datastruct);
}

DatastructItem datastruct_push(const Datastruct datastruct, const void *const new_value)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL) return NULL;

	DatastructItem new_item = malloc(sizeof(struct DatastructItem));
	new_item->next = datastruct->top;
	new_item->value = new_value;
	datastruct->top = new_item;

	return new_item;
}

void datastruct_remove(const Datastruct datastruct, const DatastructItem old_item)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL || old_item == NULL) return;

	DatastructItem prev_item = datastruct->top;
	while (prev_item && prev_item->next != old_item) {
		prev_item = prev_item->next;
	}
	// TODO: maybe we should assert?
	if (prev_item == NULL /* || prev_item->next != old_item */) return;

	prev_item->next = old_item->next;
	free(old_item);
}

DatastructItem datastruct_insert_after(
	const DatastructItem after_item,
	const void *const new_value
) {
	// TODO: maybe we should assert?
	if (NULL || after_item == NULL) return NULL;

	DatastructItem new_item = malloc(sizeof(struct DatastructItem));
	new_item->next = after_item->next;
	new_item->value = new_value;
	after_item->next = new_item;

	return new_item;
}
