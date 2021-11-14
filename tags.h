#ifndef _TAGS_H
#define _TAGS_H

#include <stdbool.h>

#define TAGS_COUNT (9)
#define TAGS_CUSTOM_NAME_SLEN (13)
#define TAGS_CUSTOM_NAME_SIZE ((TAGS_CUSTOM_NAME_SLEN) + 1)

/* compile-time check if all tags fit into an unsigned int bit array. */
struct NumTags { char limitexceeded[TAGS_COUNT > 31 ? -1 : 1]; };

struct __attribute__((packed)) TagName {
	char number;
	char colon_or_eol;
	char custom_name[TAGS_CUSTOM_NAME_SIZE];
};

struct Tag {
	bool has_custom_name;
	union {
		char cstr[sizeof(struct TagName) / sizeof(char)];
		struct TagName structured;
	} name;
};

const struct Tag *tags_get(unsigned int index);
void tags_rename(unsigned int index, const char *new_custom_name);

#endif // _TAGS_H
