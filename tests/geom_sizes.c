#include <assert.h>

#include "../src/geom.h"

static void test_create();

void test() {
	test_create();
}

void test_create()
{
	const struct Sizes sizes = sizes_create();
	assert(sizes.w == 0);
	assert(sizes.h == 0);
}
