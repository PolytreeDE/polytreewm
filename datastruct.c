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

void datastruct_push(const Datastruct datastruct, const void *const new_value)
{
	// TODO: maybe we should assert?
	if (datastruct == NULL) return;

	DatastructItem new_item = malloc(sizeof(struct DatastructItem));
	new_item->next = datastruct->top;
	new_item->value = new_value;
	datastruct->top = new_item;
}

void datastruct_remove_by_value(
	const Datastruct datastruct,
	const void *const old_value
) {
	// TODO: maybe we should assert?
	if (datastruct == NULL) return;

	DatastructItem prev_item = datastruct->top;
	while (prev_item && prev_item->next && prev_item->next->value != old_value) {
		prev_item = prev_item->next;
	}
	// TODO: maybe we should assert?
	if (prev_item == NULL ||
		prev_item->next == NULL ||
		prev_item->next->value != old_value
	) {
		return;
	}

	DatastructItem old_item = prev_item->next;
	prev_item->next = old_item->next;
	free(old_item);
}

void datastruct_insert_after_value(
	const Datastruct datastruct,
	const void *const after_value,
	const void *const new_value
) {
	// TODO: maybe we should assert?
	if (datastruct == NULL) return;

	DatastructItem after_item = datastruct->top;
	while (after_item && after_item->value != after_value) {
		after_item = after_item->next;
	}
	// TODO: maybe we should assert?
	if (after_item == NULL) return;

	DatastructItem new_item = malloc(sizeof(struct DatastructItem));
	new_item->next = after_item->next;
	new_item->value = new_value;
	after_item->next = new_item;
}
