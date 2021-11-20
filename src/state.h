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
	int isfixed, isfloating, isurgent, neverfocus, isfullscreen;
	struct ClientGeometry geometry;
};

void client_size_hints_update(
	ClientSizeHints size_hints,
	const XSizeHints *size
);

#endif // _STATE_H
