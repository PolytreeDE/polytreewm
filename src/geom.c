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

struct BasicGeometry basic_geometry_create()
{
	struct BasicGeometry basic_geometry;
	basic_geometry_init(&basic_geometry);
	return basic_geometry;
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

void basic_geometry_init(const BasicGeometry basic_geometry)
{
	position_init(&basic_geometry->position);
	sizes_init(&basic_geometry->sizes);
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

struct BasicGeometry basic_geometry_create_from_args(
	const int x,
	const int y,
	const int width,
	const int height
) {
	const struct BasicGeometry basic_geometry = {
		.position = position_create_from_args(x, y),
		.sizes = sizes_create_from_args(width, height),
	};
	return basic_geometry;
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

void basic_geometry_init_from_args(
	const BasicGeometry basic_geometry,
	const int x,
	const int y,
	const int width,
	const int height
) {
	position_init_from_args(&basic_geometry->position, x, y);
	sizes_init_from_args(&basic_geometry->sizes, width, height);
}
