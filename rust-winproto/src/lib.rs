use std::os::raw::*;
use std::ptr::null;

use x11::xlib::{self, Display};

struct Xbase {
    program_title: String,
    x_display: *mut Display,
    x_screen: c_int,
    x_root: c_ulong,
}

impl Xbase {
    fn new(program_title: String) -> Result<Self, Box<dyn std::error::Error>> {
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

            Ok(Self { program_title, x_display, x_screen, x_root })
        }
    }
}
