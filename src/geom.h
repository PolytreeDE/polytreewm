#ifndef _GEOM_H
#define _GEOM_H

/*****************
 * Pointer types *
 *****************/

typedef struct Position      *Position;
typedef struct Sizes         *Sizes;
typedef struct BasicGeometry *BasicGeometry;

/**************
 * Structures *
 **************/

struct Position {
	int x, y;
};

struct Sizes {
	int w, h;
};

struct BasicGeometry {
	struct Position position;
	struct Sizes sizes;
};

/****************************
 * Default create functions *
 ****************************/

struct Position position_create();
struct Sizes sizes_create();
struct BasicGeometry basic_geometry_create();

/**************************
 * Default init functions *
 **************************/

void position_init(Position position);
void sizes_init(Sizes sizes);
void basic_geometry_init(BasicGeometry basic_geometry);

/*****************************
 * Argument create functions *
 *****************************/

struct Position position_create_from_args(int x, int y);
struct Sizes sizes_create_from_args(int width, int height);

struct BasicGeometry basic_geometry_create_from_args(
	int x,
	int y,
	int width,
	int height
);

/***************************
 * Argument init functions *
 ***************************/

void position_init_from_args(Position position, int x, int y);
void sizes_init_from_args(Sizes sizes, int width, int height);

void basic_geometry_init_from_args(
	BasicGeometry basic_geometry,
	int x,
	int y,
	int width,
	int height
);

#endif // _GEOM_H
