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
	const struct Position position = position_create();
	assert(position.x == 0);
	assert(position.y == 0);
}

void test_init()
{
	struct Position position = { .x = 123, .y = 456 };
	position_init(&position);
	assert(position.x == 0);
	assert(position.y == 0);
}

void test_create_from_args()
{
	const struct Position position = position_create_from_args(123, 456);
	assert(position.x == 123);
	assert(position.y == 456);
}

void test_init_from_args()
{
	struct Position position = { .x = 0, .y = 0 };
	position_init_from_args(&position, 123, 456);
	assert(position.x == 123);
	assert(position.y == 456);
}
