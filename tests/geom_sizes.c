#include <assert.h>

#include "../src/geom.h"

static void test_create();
static void test_init();
static void test_create_from_args();
static void test_init_from_args();

void test() {
	test_create();
	test_init();
	test_create_from_args();
	test_init_from_args();
}

void test_create()
{
	const struct Sizes sizes = sizes_create();
	assert(sizes.w == 0);
	assert(sizes.h == 0);
}

void test_init()
{
	struct Sizes sizes = { .w = 123, .h = 456 };
	sizes_init(&sizes);
	assert(sizes.w == 0);
	assert(sizes.h == 0);
}

void test_create_from_args()
{
	const struct Sizes sizes = sizes_create_from_args(123, 456);
	assert(sizes.w == 123);
	assert(sizes.h == 456);
}

void test_init_from_args()
{
	struct Sizes sizes = { .w = 0, .h = 0 };
	sizes_init_from_args(&sizes, 123, 456);
	assert(sizes.w == 123);
	assert(sizes.h == 456);
}
