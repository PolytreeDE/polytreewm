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

/**************************
 * Default init functions *
 **************************/

void sizes_init(const Sizes sizes)
{
	sizes->w = 0;
	sizes->h = 0;
}

void basic_geom_init(const BasicGeom basic_geom)
{
	position_init(&basic_geom->position);
	sizes_init(&basic_geom->sizes);
}

void win_geom_init(const WinGeom win_geom)
{
	basic_geom_init(&win_geom->basic);
	win_geom->border_width = 0;
}

/*****************************
 * Argument create functions *
 *****************************/

struct Position position_create_from_args(const int x, const int y)
{
	const struct Position position = { .x = x, .y = y };
	return position;
}

struct Sizes sizes_create_from_args(const int width, const int height)
{
	const struct Sizes sizes = { .w = width, .h = height };
	return sizes;
}

struct BasicGeom basic_geom_create_from_args(
	const int x,
	const int y,
	const int width,
	const int height
) {
	const struct BasicGeom basic_geom = {
		.position = position_create_from_args(x, y),
		.sizes = sizes_create_from_args(width, height),
	};
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

/***************************
 * Argument init functions *
 ***************************/

void position_init_from_args(const Position position, const int x, const int y)
{
	position->x = x;
	position->y = y;
}

void sizes_init_from_args(const Sizes sizes, const int width, const int height)
{
	sizes->w = width;
	sizes->h = height;
}

void basic_geom_init_from_args(
	const BasicGeom basic_geom,
	const int x,
	const int y,
	const int width,
	const int height
) {
	position_init_from_args(&basic_geom->position, x, y);
	sizes_init_from_args(&basic_geom->sizes, width, height);
}

void win_geom_init_from_args(
	const WinGeom win_geom,
	const int x,
	const int y,
	const int width,
	const int height,
	const int border_width
) {
	basic_geom_init_from_args(&win_geom->basic, x, y, width, height);
	win_geom->border_width = border_width;
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

/**********************
 * Constant functions *
 **********************/

int win_geom_total_width(
	const struct WinGeom *const win_geom
) {
	return win_geom->basic.sizes.w + 2 * win_geom->border_width;
}

int win_geom_total_height(
	const struct WinGeom *const win_geom
) {
	return win_geom->basic.sizes.h + 2 * win_geom->border_width;
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
