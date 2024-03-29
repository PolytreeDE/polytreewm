#include "geom.h"

/****************************
 * Default create functions *
 ****************************/

struct Position position_create()
{
	struct Position position;
	position_init(&position);
	return position;
}

struct Sizes sizes_create()
{
	struct Sizes sizes;
	sizes_init(&sizes);
	return sizes;
}

struct BasicGeom basic_geom_create()
{
	struct BasicGeom basic_geom;
	basic_geom_init(&basic_geom);
	return basic_geom;
}

struct WinGeom win_geom_create()
{
	struct WinGeom win_geom;
	win_geom_init(&win_geom);
	return win_geom;
}

/*****************************
 * Argument create functions *
 *****************************/

struct Position position_create_from_args(const int x, const int y)
{
	struct Position position;
	position_init_from_args(&position, x, y);
	return position;
}

struct Sizes sizes_create_from_args(const int width, const int height)
{
	struct Sizes sizes;
	sizes_init_from_args(&sizes, width, height);
	return sizes;
}

struct BasicGeom basic_geom_create_from_args(
	const int x,
	const int y,
	const int width,
	const int height
) {
	struct BasicGeom basic_geom;
	basic_geom_init_from_args(&basic_geom, x, y, width, height);
	return basic_geom;
}

struct WinGeom win_geom_create_from_args(
	const int x,
	const int y,
	const int width,
	const int height,
	const int border_width
) {
	struct WinGeom win_geom;
	win_geom_init_from_args(&win_geom, x, y, width, height, border_width);
	return win_geom;
}

/************************
 * Conversion functions *
 ************************/

void win_geom_convert_to_x_window_changes(
	const struct WinGeom *const win_geom,
	XWindowChanges *const x_window_changes
) {
	x_window_changes->x            = win_geom->basic.position.x;
	x_window_changes->y            = win_geom->basic.position.y;
	x_window_changes->width        = win_geom->basic.sizes.w;
	x_window_changes->height       = win_geom->basic.sizes.h;
	x_window_changes->border_width = win_geom->border_width;
}

/***********************
 * Modifying functions *
 ***********************/

void win_geom_adjust_to_boundary(
	const WinGeom win_geom,
	const struct BasicGeom *const boundary_geom
) {
	const int total_width  = win_geom_total_width(win_geom);
	const int total_height = win_geom_total_height(win_geom);

	if (
		win_geom->basic.position.x + total_width
		>
		boundary_geom->position.x + boundary_geom->sizes.w
	) {
		win_geom->basic.position.x =
			boundary_geom->position.x
			+
			boundary_geom->sizes.w
			-
			total_width;
	}

	if (
		win_geom->basic.position.y + total_height
		>
		boundary_geom->position.y + boundary_geom->sizes.h
	) {
		win_geom->basic.position.y =
			boundary_geom->position.y
			+
			boundary_geom->sizes.h
			-
			total_height;
	}

	if (win_geom->basic.position.x < boundary_geom->position.x) {
		win_geom->basic.position.x = boundary_geom->position.x;
	}

	if (win_geom->basic.position.y < boundary_geom->position.y) {
		win_geom->basic.position.y = boundary_geom->position.y;
	}
}
