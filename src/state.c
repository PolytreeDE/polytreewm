#include "state.h"

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
