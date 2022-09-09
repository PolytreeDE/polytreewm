use crate::*;

use std::alloc::{alloc, dealloc, Layout};

use x11::xlib::Display;

#[no_mangle]
unsafe extern "C" fn atoms_create(dpy: *mut Display) -> *mut Atoms {
    let layout = Layout::new::<Atoms>();
    let ptr = alloc(layout);
    *(ptr as *mut Atoms) = Atoms::create(dpy);
    ptr as *mut Atoms
}

#[no_mangle]
unsafe extern "C" fn atoms_delete(atoms: *mut Atoms) {
    let layout = Layout::new::<Atoms>();
    dealloc(atoms as *mut u8, layout);
}
