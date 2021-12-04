#ifndef _GEOM_H
#define _GEOM_H

#include <X11/Xutil.h>

/*****************
 * Pointer types *
 *****************/

typedef struct Position  *Position;
typedef struct Sizes     *Sizes;
typedef struct BasicGeom *BasicGeom;
typedef struct WinGeom   *WinGeom;

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

struct WinGeom {
	struct BasicGeom basic;
	int border_width;
};

/****************************
 * Default create functions *
 ****************************/

struct Position position_create();
struct Sizes sizes_create();
struct BasicGeom basic_geom_create();
struct WinGeom win_geom_create();

/**************************
 * Default init functions *
 **************************/

void position_init(Position position);
void sizes_init(Sizes sizes);
void basic_geom_init(BasicGeom basic_geom);
void win_geom_init(WinGeom win_geom);

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

struct WinGeom win_geom_create_from_args(
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

void win_geom_init_from_args(
	WinGeom win_geom,
	int x,
	int y,
	int width,
	int height,
	int border_width
);

/************************
 * Conversion functions *
 ************************/

void win_geom_convert_to_x_window_changes(
	const struct WinGeom *win_geom,
	XWindowChanges *x_window_changes
);

/**********************
 * Constant functions *
 **********************/

int win_geom_total_width(const struct WinGeom *win_geom);
int win_geom_total_height(const struct WinGeom *win_geom);

/***********************
 * Modifying functions *
 ***********************/

void win_geom_adjust_to_boundary(
	WinGeom win_geom,
	const struct BasicGeom *boundary_geom
);

#endif // _GEOM_H
