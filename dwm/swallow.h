#ifndef _DWM_SWALLOW_H
#define _DWM_SWALLOW_H

static pid_t getparentprocess(pid_t p);
static int isdescprocess(pid_t p, pid_t c);
static void swallow(Client *p, Client *c);
static Client *swallowingclient(Window w);
static Client *termforwin(const Client *c);
static void unswallow(Client *c);
static pid_t winpid(Window w);

#endif // _DWM_SWALLOW_H
