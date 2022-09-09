#ifndef _DWM_H
#define _DWM_H

#include "config/buttons.h"
#include "config/cursors.h"
#include "config/keys.h"
#include "config/layouts.h"

int dwm_main(const char *program_title);

void centeredmaster(void *arg);
void floating(void *arg);
void horizontile(void *arg);
void monocle(void *arg);
void tile(void *arg);

void configborder(const Arg *arg);
void configgap(const Arg *arg);
void dorestart(const Arg *arg);
void focusmon(const Arg *arg);
void focusstack(const Arg *arg);
void incnmaster(const Arg *arg);
void killclient(const Arg *arg);
void movemouse(const Arg *arg);
void movestack(const Arg *arg);
void quit(const Arg *arg);
void resetnmaster(const Arg *arg);
void resizemouse(const Arg *arg);
void setlayout(const Arg *arg);
void setmfact(const Arg *arg);
void spawn(const Arg *arg);
void tagmon(const Arg *arg);
void togglefloating(const Arg *arg);
void zoom(const Arg *arg);

#endif // _DWM_H
