#ifndef _DWM_TYPES_H
#define _DWM_TYPES_H

enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel }; /* color schemes */
enum { ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

typedef struct Monitor Monitor;
typedef struct Client Client;

typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

typedef struct {
	unsigned int click;
	unsigned int mask;
	unsigned int button;
	void (*func)(const Arg *arg);
	const Arg arg;
} Button;

struct Client {
	struct ClientState state;
	struct ClientSizeHints size_hints;

	Client *next;
	Client *snext;
	Monitor *mon;
	Window x_window;
};

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

typedef struct {
	void (*arrange)(Monitor *);
} Layout;

struct Monitor {
	struct BasicGeom screen_geom;
	struct BasicGeom window_area_geom;
	Unit unit;

	int nmaster;
	int num;
	unsigned int sellt;
	Client *clients;
	Client *sel;
	Client *stack;
	Monitor *next;
	const Layout *lt[2];
};

#endif // _DWM_TYPES_H
