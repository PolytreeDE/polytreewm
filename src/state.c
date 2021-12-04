#include "state.h"

#include <string.h>

/**************************
 * Default init functions *
 **************************/

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
	win_geom_init(&client_state->geom);
	client_state->is_fixed = false;
	client_state->is_floating = false;
	client_state->is_urgent = false;
	client_state->never_focus = false;
	client_state->is_fullscreen = false;
}

/***********************
 * Modifying functions *
 ***********************/

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
