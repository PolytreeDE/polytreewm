#ifndef _STATE_H
#define _STATE_H

#include <stdbool.h>
#include <X11/Xutil.h>

/*****************
 * Pointer types *
 *****************/

typedef struct BasicGeometry *BasicGeometry;
typedef struct ClientGeometry *ClientGeometry;
typedef struct ClientSizeHints *ClientSizeHints;
typedef struct ClientState *ClientState;

/**************
 * Structures *
 **************/

struct BasicGeometry { int x, y, w, h; };

struct ClientGeometry {
	struct BasicGeometry basic;
	int bw;
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

/*************
 * Functions *
 *************/

void basic_geometry_init(BasicGeometry basic_geometry);
void client_geometry_init(ClientGeometry client_geometry);
void client_size_hints_init(ClientSizeHints client_size_hints);
void client_state_init(ClientState client_state);

void client_size_hints_update(
	ClientSizeHints size_hints,
	const XSizeHints *size
);

#endif // _STATE_H
