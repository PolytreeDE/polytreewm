#ifndef _DRW_H
#define _DRW_H

enum { ColFg, ColBg, ColBorder }; /* Clr scheme index */
typedef XftColor Clr;

typedef struct {
	unsigned int w, h;
	Display *dpy;
	int screen;
	Window root;
	Drawable drawable;
	GC gc;
	Clr *scheme;
} Drw;

/* Drawable abstraction */
Drw *drw_create(Display *dpy, int screen, Window win, unsigned int w, unsigned int h);
void drw_free(Drw *drw);

/* Colorscheme abstraction */
Clr *drw_scm_create(Drw *drw, const char *clrnames[], size_t clrcount);

#endif // _DRW_H
