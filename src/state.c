#include "state.h"

#include <string.h>

/**************************
 * Default init functions *
 **************************/

void client_geom_init(const ClientGeom client_geom)
{
	basic_geom_init(&client_geom->basic);
	client_geom->border_width = 0;
}

void client_size_hints_init(const ClientSizeHints client_size_hints)
{
	client_size_hints->mina = 0;
	client_size_hints->maxa = 0;
	client_size_hints->basew = 0;
	client_size_hints->baseh = 0;
	client_size_hints->incw = 0;
	client_size_hints->inch = 0;
	client_size_hints->maxw = 0;
	client_size_hints->maxh = 0;
	client_size_hints->minw = 0;
	client_size_hints->minh = 0;
}

void client_state_init(const ClientState client_state)
{
	memset(client_state->name, 0, sizeof(client_state->name));
	client_geom_init(&client_state->geom);
	client_state->is_fixed = false;
	client_state->is_floating = false;
	client_state->is_urgent = false;
	client_state->never_focus = false;
	client_state->is_fullscreen = false;
}

/***************************
 * Argument init functions *
 ***************************/

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

void client_size_hints_update(
	const ClientSizeHints size_hints,
	const XSizeHints *const size
) {
	if (size->flags & PBaseSize) {
		size_hints->basew = size->base_width;
		size_hints->baseh = size->base_height;
	} else if (size->flags & PMinSize) {
		size_hints->basew = size->min_width;
		size_hints->baseh = size->min_height;
	} else {
		size_hints->basew = size_hints->baseh = 0;
	}

	if (size->flags & PResizeInc) {
		size_hints->incw = size->width_inc;
		size_hints->inch = size->height_inc;
	} else {
		size_hints->incw = size_hints->inch = 0;
	}

	if (size->flags & PMaxSize) {
		size_hints->maxw = size->max_width;
		size_hints->maxh = size->max_height;
	} else {
		size_hints->maxw = size_hints->maxh = 0;
	}

	if (size->flags & PMinSize) {
		size_hints->minw = size->min_width;
		size_hints->minh = size->min_height;
	} else if (size->flags & PBaseSize) {
		size_hints->minw = size->base_width;
		size_hints->minh = size->base_height;
	} else {
		size_hints->minw = size_hints->minh = 0;
	}

	if (size->flags & PAspect) {
		size_hints->mina = (float)size->min_aspect.y / size->min_aspect.x;
		size_hints->maxa = (float)size->max_aspect.x / size->max_aspect.y;
	} else {
		size_hints->maxa = size_hints->mina = 0.0;
	}
}
