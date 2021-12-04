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
	const struct BasicGeom basic_geom = basic_geom_create();
	assert(basic_geom.position.x == 0);
	assert(basic_geom.position.y == 0);
	assert(basic_geom.sizes.w == 0);
	assert(basic_geom.sizes.h == 0);
}

void test_init()
{
	struct BasicGeom basic_geom = {
		.position = { .x = 12, .y = 34 },
		.sizes    = { .w = 56, .h = 68 },
	};
	basic_geom_init(&basic_geom);
	assert(basic_geom.position.x == 0);
	assert(basic_geom.position.y == 0);
	assert(basic_geom.sizes.w == 0);
	assert(basic_geom.sizes.h == 0);
}

void test_create_from_args()
{
	const struct BasicGeom basic_geom =
		basic_geom_create_from_args(12, 34, 56, 78);
	assert(basic_geom.position.x == 12);
	assert(basic_geom.position.y == 34);
	assert(basic_geom.sizes.w == 56);
	assert(basic_geom.sizes.h == 78);
}

void test_init_from_args()
{
	struct BasicGeom basic_geom = {
		.position = { .x = 0, .y = 0 },
		.sizes    = { .w = 0, .h = 0 },
	};
	basic_geom_init_from_args(&basic_geom, 12, 34, 56, 78);
	assert(basic_geom.position.x == 12);
	assert(basic_geom.position.y == 34);
	assert(basic_geom.sizes.w == 56);
	assert(basic_geom.sizes.h == 78);
}
