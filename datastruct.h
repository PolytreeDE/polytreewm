#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H

#include <stddef.h>

#define DATASTRUCT_DELETE(datastruct) { \
	datastruct_delete(datastruct);      \
	datastruct = NULL;                  \
}

typedef struct Datastruct *Datastruct;

Datastruct datastruct_new();
void datastruct_delete(Datastruct datastruct);

#endif // _DATASTRUCT_H
