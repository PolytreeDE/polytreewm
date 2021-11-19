#ifndef _DWM_HANDLERS_H
#define _DWM_HANDLERS_H

static void on_button_press(XEvent *e);
static void on_client_message(XEvent *e);
static void on_configure_notify(XEvent *e);
static void on_configure_request(XEvent *e);
static void on_destroy_notify(XEvent *e);
static void on_expose(XEvent *e);
static void on_focus_in(XEvent *e);
static void on_key_press(XEvent *e);
static void on_mapping_notify(XEvent *e);
static void on_map_request(XEvent *e);
static void on_property_notify(XEvent *e);
static void on_unmap_notify(XEvent *e);

#endif // _DWM_HANDLERS_H
