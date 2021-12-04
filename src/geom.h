#ifndef _GEOM_H
#define _GEOM_H

#include <X11/Xutil.h>

/*****************
 * Pointer types *
 *****************/

typedef struct Position   *Position;
typedef struct Sizes      *Sizes;
typedef struct BasicGeom  *BasicGeom;
typedef struct ClientGeom *ClientGeom;

/**************
 * Structures *
 **************/

struct Position {
	int x, y;
};

struct Sizes {
	int w, h;
};

struct BasicGeom {
	struct Position position;
	struct Sizes sizes;
};

struct ClientGeom {
	struct BasicGeom basic;
	int border_width;
};

/****************************
 * Default create functions *
 ****************************/

struct Position position_create();
struct Sizes sizes_create();
struct BasicGeom basic_geom_create();
struct ClientGeom client_geom_create();

/**************************
 * Default init functions *
 **************************/

void position_init(Position position);
void sizes_init(Sizes sizes);
void basic_geom_init(BasicGeom basic_geom);
void client_geom_init(ClientGeom client_geom);

/*****************************
 * Argument create functions *
 *****************************/

struct Position position_create_from_args(int x, int y);
struct Sizes sizes_create_from_args(int width, int height);

struct BasicGeom basic_geom_create_from_args(
	int x,
	int y,
	int width,
	int height
);

struct ClientGeom client_geom_create_from_args(
	int x,
	int y,
	int width,
	int height,
	int border_width
);

/***************************
 * Argument init functions *
 ***************************/

void position_init_from_args(Position position, int x, int y);
void sizes_init_from_args(Sizes sizes, int width, int height);

void basic_geom_init_from_args(
	BasicGeom basic_geom,
	int x,
	int y,
	int width,
	int height
);

void client_geom_init_from_args(
	ClientGeom client_geom,
	int x,
	int y,
	int width,
	int height,
	int border_width
);

/************************
 * Conversion functions *
 ************************/

void client_geom_convert_to_x_window_changes(
	const struct ClientGeom *client_geom,
	XWindowChanges *x_window_changes
);

/**********************
 * Constant functions *
 **********************/

int client_geom_total_width(const struct ClientGeom *client_geom);
int client_geom_total_height(const struct ClientGeom *client_geom);

/***********************
 * Modifying functions *
 ***********************/

void client_geom_adjust_to_boundary(
	ClientGeom client_geom,
	const struct BasicGeom *boundary_geom
);

#endif // _GEOM_H
