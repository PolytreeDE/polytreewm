#ifndef _DWM_SYSTRAY_H
#define _DWM_SYSTRAY_H

static unsigned int getsystraywidth();
static void removesystrayicon(Client *i);
static Monitor *systraytomon(Monitor *m);
static void updatesystray(void);
static void updatesystrayicongeom(Client *i, int w, int h);
static void updatesystrayiconstate(Client *i, XPropertyEvent *ev);
static Client *wintosystrayicon(Window w);

#endif // _DWM_SYSTRAY_H
