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
