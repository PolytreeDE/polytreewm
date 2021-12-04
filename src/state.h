#ifndef _STATE_H
#define _STATE_H

#include "geom.h"

#include <stdbool.h>
#include <X11/Xutil.h>

/*****************
 * Pointer types *
 *****************/

typedef struct ClientGeom      *ClientGeom;
typedef struct ClientSizeHints *ClientSizeHints;
typedef struct ClientState     *ClientState;

/**************
 * Structures *
 **************/

struct ClientGeom {
	struct BasicGeom basic;
	int border_width;
};

struct ClientSizeHints {
	float mina, maxa;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
};

struct ClientState {
	char name[256];
	struct ClientGeom geom;
	bool is_fixed, is_floating, is_urgent, never_focus, is_fullscreen;
};

/**************************
 * Default init functions *
 **************************/

void client_geom_init(ClientGeom client_geom);
void client_size_hints_init(ClientSizeHints client_size_hints);
void client_state_init(ClientState client_state);

/***************************
 * Argument init functions *
 ***************************/

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

void client_size_hints_update(
	ClientSizeHints size_hints,
	const XSizeHints *size
);

#endif // _STATE_H
