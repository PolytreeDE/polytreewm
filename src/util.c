#include "util.h"

#include "main.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *ecalloc(size_t nmemb, size_t size)
{
	void *p = calloc(nmemb, size);
	if (!p) die_perror("calloc");
	return p;
}
