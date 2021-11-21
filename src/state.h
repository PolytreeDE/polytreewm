#ifndef _STATE_H
#define _STATE_H

#include <stdbool.h>
#include <X11/Xutil.h>

/*****************
 * Pointer types *
 *****************/

typedef struct Position        *Position;
typedef struct Sizes           *Sizes;
typedef struct BasicGeometry   *BasicGeometry;
typedef struct ClientGeometry  *ClientGeometry;
typedef struct ClientSizeHints *ClientSizeHints;
typedef struct ClientState     *ClientState;

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

struct ClientGeometry {
	struct BasicGeometry basic;
	int border_width;
};

struct ClientSizeHints {
	float mina, maxa;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
};

struct ClientState {
	char name[256];
	struct ClientGeometry geometry;
	bool is_fixed, is_floating, is_urgent, never_focus, is_fullscreen;
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
void client_geometry_init(ClientGeometry client_geometry);
void client_size_hints_init(ClientSizeHints client_size_hints);
void client_state_init(ClientState client_state);

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

void client_geometry_init_from_args(
	ClientGeometry client_geometry,
	int x,
	int y,
	int width,
	int height,
	int border_width
);

/************************
 * Conversion functions *
 ************************/

void client_geometry_convert_to_x_window_changes(
	const struct ClientGeometry *client_geometry,
	XWindowChanges *x_window_changes
);

/**********************
 * Constant functions *
 **********************/

int client_geometry_total_width(const struct ClientGeometry *client_geometry);
int client_geometry_total_height(const struct ClientGeometry *client_geometry);

/***********************
 * Modifying functions *
 ***********************/

void client_geometry_adjust_to_boundary(
	ClientGeometry client_geometry,
	const struct BasicGeometry *boundary_geometry
);

void client_size_hints_update(
	ClientSizeHints size_hints,
	const XSizeHints *size
);

#endif // _STATE_H