#ifndef _STATE_H
#define _STATE_H

#include <X11/Xutil.h>

struct BasicGeometry {
	int x, y, w, h;
};

struct ClientGeometry {
	struct BasicGeometry basic;
	int bw;
};

typedef struct ClientSizeHints {
	float mina, maxa;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
} *ClientSizeHints;

struct ClientState {
	char name[256];
	struct ClientGeometry geometry;
	int is_fixed, is_floating, is_urgent, never_focus, is_fullscreen;
};

void client_size_hints_update(
	ClientSizeHints size_hints,
	const XSizeHints *size
);

#endif // _STATE_H
