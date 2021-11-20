#ifndef _STATE_H
#define _STATE_H

struct BasicGeometry {
	int x, y, w, h;
};

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
	int isfixed, isfloating, isurgent, neverfocus, isfullscreen;
	struct ClientGeometry geometry;
};

#endif // _STATE_H
