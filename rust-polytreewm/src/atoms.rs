use x11::xlib::{self, Atom, Display, False};

#[repr(C)]
pub struct Atoms {
    wmatom: [Atom; 4],
    netatom: [Atom; 9],
    utf8string: Atom,
}

fn atom(dpy: *mut Display, name: &str) -> Atom {
    unsafe { xlib::XInternAtom(dpy, name.as_ptr() as *const i8, False) }
}

impl Atoms {
    pub fn create(dpy: *mut Display) -> Self {
        Self {
            wmatom: [
                atom(dpy, "WM_PROTOCOLS"),
                atom(dpy, "WM_DELETE_WINDOW"),
                atom(dpy, "WM_STATE"),
                atom(dpy, "WM_TAKE_FOCUS"),
            ],
            netatom: [
                atom(dpy, "_NET_SUPPORTED"),
                atom(dpy, "_NET_WM_NAME"),
                atom(dpy, "_NET_WM_STATE"),
                atom(dpy, "_NET_SUPPORTING_WM_CHECK"),
                atom(dpy, "_NET_WM_STATE_FULLSCREEN"),
                atom(dpy, "_NET_ACTIVE_WINDOW"),
                atom(dpy, "_NET_WM_WINDOW_TYPE"),
                atom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG"),
                atom(dpy, " _NET_CLIENT_LIST"),
            ],
            utf8string: atom(dpy, "UTF8_STRING"),
        }
    }
}
