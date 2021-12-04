#include <assert.h>

#include "../src/geom.h"

static void test_create();

void test() {
	test_create();
}

void test_create()
{
	const struct Position position = position_create();
	assert(position.x == 0);
	assert(position.y == 0);
}
