#include "datastruct.h"

#include <stdlib.h>

typedef struct Item {
	struct Item *next;
	const void *value;
} *Item;

struct Datastruct {
	Item top;
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

	for (Item item = datastruct->top; item; item = item->next) free(item);
	free(datastruct);
}

void datastruct_insert_after_value(
	const Datastruct datastruct,
	const void *const after_value,
	const void *const new_value
) {
	// TODO: maybe we should assert?
	if (datastruct == NULL) return;

	Item after_item = datastruct->top;
	while (after_item && after_item->value != after_value) {
		after_item = after_item->next;
	}
	// TODO: maybe we should assert?
	if (after_item == NULL) return;

	Item new_item = malloc(sizeof(struct Item));
	new_item->next = after_item->next;
	new_item->value = new_value;
	after_item->next = new_item;
}
