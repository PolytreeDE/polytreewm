#include "tags.h"

#include <stddef.h>
#include <string.h>

static struct Tag tags[TAGS_COUNT] = {
	[0] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '1', .colon_or_eol = '\0' } },
	},
	[1] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '2', .colon_or_eol = '\0' } },
	},
	[2] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '3', .colon_or_eol = '\0' } },
	},
	[3] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '4', .colon_or_eol = '\0' } },
	},
	[4] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '5', .colon_or_eol = '\0' } },
	},
	[5] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '6', .colon_or_eol = '\0' } },
	},
	[6] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '7', .colon_or_eol = '\0' } },
	},
	[7] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '8', .colon_or_eol = '\0' } },
	},
	[8] = {
		.has_custom_name = false,
		.name = { .structured = { .number = '9', .colon_or_eol = '\0' } },
	},
};

const struct Tag *tags_get(const unsigned int index)
{
	if (index >= TAGS_COUNT) return NULL;

	return &tags[index];
}

void tags_rename(const unsigned int index, const char *const new_custom_name)
{
	if (index >= TAGS_COUNT) return;

	if (new_custom_name == NULL || new_custom_name[0] == '\0') {
		tags[index].has_custom_name = false;
		tags[index].name.structured.colon_or_eol = '\0';
	} else {
		tags[index].has_custom_name = true;
		tags[index].name.structured.colon_or_eol = ':';
		strncpy(tags[index].name.structured.custom_name, new_custom_name, TAGS_CUSTOM_NAME_SIZE);
		tags[index].name.structured.custom_name[TAGS_CUSTOM_NAME_SIZE - 1] = '\0';
	}
}
