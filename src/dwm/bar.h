#ifndef _DWM_BAR_H
#define _DWM_BAR_H

static void createbars(void);
static void drawbar(Monitor *m);
static void drawbars(void);
static void resizebarwin(Monitor *m);
static void togglebar(const Arg *arg);
static void updatebarpos(Monitor *m);
static void updatebar(Monitor *m);
static void updatebars();

#endif // _DWM_BAR_H
