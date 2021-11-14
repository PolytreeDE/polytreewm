#include "datastruct.h"

#include <stdlib.h>

typedef struct Item {
	struct Item *next;
	void *value;
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

	free(datastruct);
}
