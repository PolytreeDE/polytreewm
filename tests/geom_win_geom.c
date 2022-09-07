#include <assert.h>

#include "../src/geom.h"

static void test_total_width_height();

void test() {
	test_total_width_height();
}

void test_total_width_height()
{
	const struct WinGeom win_geom = win_geom_create_from_args(0, 0, 34, 56, 12);
	assert(win_geom_total_width(&win_geom) == 58);
	assert(win_geom_total_height(&win_geom) == 80);
}
