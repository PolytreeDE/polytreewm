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

struct ClientGeom client_geom_create()
{
	struct ClientGeom client_geom;
	client_geom_init(&client_geom);
	return client_geom;
}

/**************************
 * Default init functions *
 **************************/

void position_init(const Position position)
{
	position->x = 0;
	position->y = 0;
}

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

void client_geom_init(const ClientGeom client_geom)
{
	basic_geom_init(&client_geom->basic);
	client_geom->border_width = 0;
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

struct ClientGeom client_geom_create_from_args(
	const int x,
	const int y,
	const int width,
	const int height,
	const int border_width
) {
	struct ClientGeom client_geom;
	client_geom_init_from_args(&client_geom, x, y, width, height, border_width);
	return client_geom;
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

void client_geom_init_from_args(
	const ClientGeom client_geom,
	const int x,
	const int y,
	const int width,
	const int height,
	const int border_width
) {
	basic_geom_init_from_args(&client_geom->basic, x, y, width, height);
	client_geom->border_width = border_width;
}

/************************
 * Conversion functions *
 ************************/

void client_geom_convert_to_x_window_changes(
	const struct ClientGeom *const client_geom,
	XWindowChanges *const x_window_changes
) {
	x_window_changes->x            = client_geom->basic.position.x;
	x_window_changes->y            = client_geom->basic.position.y;
	x_window_changes->width        = client_geom->basic.sizes.w;
	x_window_changes->height       = client_geom->basic.sizes.h;
	x_window_changes->border_width = client_geom->border_width;
}

/**********************
 * Constant functions *
 **********************/

int client_geom_total_width(
	const struct ClientGeom *const client_geom
) {
	return client_geom->basic.sizes.w + 2 * client_geom->border_width;
}

int client_geom_total_height(
	const struct ClientGeom *const client_geom
) {
	return client_geom->basic.sizes.h + 2 * client_geom->border_width;
}

/***********************
 * Modifying functions *
 ***********************/

void client_geom_adjust_to_boundary(
	const ClientGeom client_geom,
	const struct BasicGeom *const boundary_geom
) {
	const int total_width  = client_geom_total_width(client_geom);
	const int total_height = client_geom_total_height(client_geom);

	if (
		client_geom->basic.position.x + total_width
		>
		boundary_geom->position.x + boundary_geom->sizes.w
	) {
		client_geom->basic.position.x =
			boundary_geom->position.x
			+
			boundary_geom->sizes.w
			-
			total_width;
	}

	if (
		client_geom->basic.position.y + total_height
		>
		boundary_geom->position.y + boundary_geom->sizes.h
	) {
		client_geom->basic.position.y =
			boundary_geom->position.y
			+
			boundary_geom->sizes.h
			-
			total_height;
	}

	if (client_geom->basic.position.x < boundary_geom->position.x) {
		client_geom->basic.position.x = boundary_geom->position.x;
	}

	if (client_geom->basic.position.y < boundary_geom->position.y) {
		client_geom->basic.position.y = boundary_geom->position.y;
	}
}
