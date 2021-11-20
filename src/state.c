#include "state.h"

#include <string.h>

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

void client_geometry_init(const ClientGeometry client_geometry)
{
	basic_geometry_init(&client_geometry->basic);
	client_geometry->border_width = 0;
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
	client_geometry_init(&client_state->geometry);
	client_state->is_fixed = false;
	client_state->is_floating = false;
	client_state->is_urgent = false;
	client_state->never_focus = false;
	client_state->is_fullscreen = false;
}

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

void client_geometry_init_from_args(
	const ClientGeometry client_geometry,
	int x,
	int y,
	int width,
	int height,
	int border_width
) {
	position_init_from_args(&client_geometry->basic.position, x, y);
	sizes_init_from_args(&client_geometry->basic.sizes, width, height);
	client_geometry->border_width = border_width;
}

int client_geometry_total_width(
	const struct ClientGeometry *const client_geometry
) {
	return client_geometry->basic.sizes.w + 2 * client_geometry->border_width;
}

int client_geometry_total_height(
	const struct ClientGeometry *const client_geometry
) {
	return client_geometry->basic.sizes.h + 2 * client_geometry->border_width;
}

void client_geometry_to_x_window_changes(
	const struct ClientGeometry *const client_geometry,
	XWindowChanges *const x_window_changes
) {
	x_window_changes->x            = client_geometry->basic.position.x;
	x_window_changes->y            = client_geometry->basic.position.y;
	x_window_changes->width        = client_geometry->basic.sizes.w;
	x_window_changes->height       = client_geometry->basic.sizes.h;
	x_window_changes->border_width = client_geometry->border_width;
}

void client_geometry_adjust_to_boundary(
	const ClientGeometry client_geometry,
	const struct BasicGeometry *const boundary_geometry
) {
	const int total_width  = client_geometry_total_width(client_geometry);
	const int total_height = client_geometry_total_height(client_geometry);

	if (
		client_geometry->basic.position.x + total_width
		>
		boundary_geometry->position.x + boundary_geometry->sizes.w
	) {
		client_geometry->basic.position.x =
			boundary_geometry->position.x
			+
			boundary_geometry->sizes.w
			-
			total_width;
	}

	if (
		client_geometry->basic.position.y + total_height
		>
		boundary_geometry->position.y + boundary_geometry->sizes.h
	) {
		client_geometry->basic.position.y =
			boundary_geometry->position.y
			+
			boundary_geometry->sizes.h
			-
			total_height;
	}

	if (client_geometry->basic.position.x < boundary_geometry->position.x) {
		client_geometry->basic.position.x = boundary_geometry->position.x;
	}

	if (client_geometry->basic.position.y < boundary_geometry->position.y) {
		client_geometry->basic.position.y = boundary_geometry->position.y;
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
