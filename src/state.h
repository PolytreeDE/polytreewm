#ifndef _STATE_H
#define _STATE_H

#include "geom.h"

#include <stdbool.h>

/*****************
 * Pointer types *
 *****************/

typedef struct ClientSizeHints *ClientSizeHints;
typedef struct ClientState     *ClientState;

/**************
 * Structures *
 **************/

struct ClientSizeHints {
	float mina, maxa;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
};

struct ClientState {
	char name[256];
	struct WinGeom geom;
	bool is_fixed, is_floating, is_urgent, never_focus, is_fullscreen;
};

/**************************
 * Default init functions *
 **************************/

void client_size_hints_init(ClientSizeHints client_size_hints);
void client_state_init(ClientState client_state);

/***********************
 * Modifying functions *
 ***********************/

void client_size_hints_update(
	ClientSizeHints size_hints,
	const XSizeHints *size
);

#endif // _STATE_H
