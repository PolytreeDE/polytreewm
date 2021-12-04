#ifndef _STATE_H
#define _STATE_H

#include "geom.h"

#include <stdbool.h>
#include <X11/Xutil.h>

/*****************
 * Pointer types *
 *****************/

typedef struct ClientGeometry  *ClientGeometry;
typedef struct ClientSizeHints *ClientSizeHints;
typedef struct ClientState     *ClientState;

/**************
 * Structures *
 **************/

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

/**************************
 * Default init functions *
 **************************/

void client_geometry_init(ClientGeometry client_geometry);
void client_size_hints_init(ClientSizeHints client_size_hints);
void client_state_init(ClientState client_state);

/***************************
 * Argument init functions *
 ***************************/

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
