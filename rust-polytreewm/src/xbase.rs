use crate::geom;

use std::os::raw::*;
use std::ptr::null;

use x11::xlib::{self, Display};

pub type ErrorHandler =
    unsafe extern "C" fn(*mut Display, *mut xlib::XErrorEvent) -> c_int;

pub struct Xbase {
    program_title: String,
    x_display: *mut Display,
    x_screen: c_int,
    x_root: c_ulong,
    screen_sizes: geom::Sizes,
    x_error: Option<ErrorHandler>,
}

impl Xbase {
    pub fn new(
        program_title: String,
        x_error_handler: ErrorHandler,
    ) -> Result<Self, Box<dyn std::error::Error>> {
        unsafe {
            if xlib::XSupportsLocale() == 0 {
                return Err("no locale support in X".into());
            }

            let x_display = xlib::XOpenDisplay(null());
            if x_display.is_null() {
                return Err("cannot open X display".into());
            }

            let x_screen = xlib::XDefaultScreen(x_display);
            let x_root = xlib::XRootWindow(x_display, x_screen);

            let screen_sizes = geom::Sizes::new(
                xlib::XDisplayWidth(x_display, x_screen),
                xlib::XDisplayHeight(x_display, x_screen),
            );

            let x_error = xlib::XSetErrorHandler(Some(x_error_wm_check));

            xlib::XSelectInput(
                x_display,
                xlib::XDefaultRootWindow(x_display),
                xlib::SubstructureRedirectMask,
            );
            xlib::XSync(x_display, xlib::False);
            xlib::XSetErrorHandler(Some(x_error_handler));
            xlib::XSync(x_display, xlib::False);

            Ok(Self {
                program_title,
                x_display,
                x_screen,
                x_root,
                screen_sizes,
                x_error,
            })
        }
    }
}

impl Drop for Xbase {
    fn drop(&mut self) {
        unsafe { xlib::XCloseDisplay(self.x_display) };
    }
}

unsafe extern "C" fn x_error_wm_check(
    _display: *mut Display,
    _x_error_event: *mut xlib::XErrorEvent,
) -> c_int {
    panic!("another window manager is already running");
}
