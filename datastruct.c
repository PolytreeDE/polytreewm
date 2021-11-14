#include "datastruct.h"

#include <stdlib.h>

struct Datastruct {
	DatastructItem top;
};

struct DatastructItem {
	Datastruct datastruct;
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

void *datastruct_value(const Datastruct datastruct, const DatastructItem item)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL || item == NULL || item->datastruct != datastruct) {
		return NULL;
	}

	// We discard const modifier because user may want to modify it's data.
	return (void*)item->value;
}

DatastructItem datastruct_top(const Datastruct datastruct)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL) return NULL;

	return datastruct->top;
}

DatastructItem datastruct_next(
	const Datastruct datastruct,
	const DatastructItem item
) {
	// TODO: maybe we should assert?
	if (datastruct == NULL || item == NULL || item->datastruct != datastruct) {
		return NULL;
	}

	return item->next;
}

DatastructItem datastruct_find_first(
	const Datastruct datastruct,
	const void *const value
) {
	// TODO: maybe we should assert?
	if (datastruct == NULL) return NULL;

	for (DatastructItem item = datastruct->top; item; item = item->next) {
		if (item->value == value) return item;
	}

	return NULL;
}

DatastructItem datastruct_find_last(
	const Datastruct datastruct,
	const void *const value
) {
	// TODO: maybe we should assert?
	if (datastruct == NULL) return NULL;

	DatastructItem result = NULL;

	for (DatastructItem item = datastruct->top; item; item = item->next) {
		if (item->value == value) result = item;
	}

	return result;
}

void datastruct_remove(const Datastruct datastruct, const DatastructItem item)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL || item == NULL || item->datastruct != datastruct) {
		return;
	}

	DatastructItem prev_item = datastruct->top;
	while (prev_item && prev_item->next != item) {
		prev_item = prev_item->next;
	}
	// TODO: maybe we should assert?
	if (prev_item == NULL) return;

	prev_item->next = item->next;
	free(item);
}

DatastructItem datastruct_push(const Datastruct datastruct, const void *const value)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL) return NULL;

	DatastructItem item = malloc(sizeof(struct DatastructItem));
	item->datastruct = datastruct;
	item->next = datastruct->top;
	item->value = value;
	datastruct->top = item;
	return item;
}

DatastructItem datastruct_insert(
	const Datastruct datastruct,
	const DatastructItem item,
	const void *const value
) {
	// TODO: maybe we should assert?
	if (datastruct == NULL || item == NULL || item->datastruct != datastruct) {
		return NULL;
	}

	DatastructItem new_item = malloc(sizeof(struct DatastructItem));
	new_item->datastruct = datastruct;
	new_item->next = item->next;
	new_item->value = value;
	item->next = new_item;
	return new_item;
}
