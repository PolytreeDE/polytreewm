#ifndef _LAYOUTS_H
#define _LAYOUTS_H

#include <stddef.h>

typedef struct {
	void (*arrange)(void*);
} Layout;

extern const Layout layouts[];

size_t layouts_count();

#endif // _LAYOUTS_H
