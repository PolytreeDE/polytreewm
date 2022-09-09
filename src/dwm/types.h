#ifndef _DWM_TYPES_H
#define _DWM_TYPES_H

enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel }; /* color schemes */

typedef struct Monitor Monitor;
typedef struct Client Client;

struct Client {
	struct ClientState state;
	struct ClientSizeHints size_hints;

	Client *next;
	Client *snext;
	Monitor *mon;
	Window x_window;
};

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
