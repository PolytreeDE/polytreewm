use crate::*;

use std::alloc::{alloc, dealloc, Layout};
use std::ffi::CStr;
use std::os::raw::*;

#[no_mangle]
unsafe extern "C" fn xbase_new(
    program_title: *const c_char,
    _x_error_handler: xbase::ErrorHandler,
) -> *mut Xbase {
    let program_title = CStr::from_ptr(program_title).to_str().unwrap();
    let layout = Layout::new::<Xbase>();
    let ptr = alloc(layout) as *mut Xbase;
    *ptr = Xbase::new(program_title.to_string() /*, x_error_handler*/).unwrap();
    ptr
}

#[no_mangle]
unsafe extern "C" fn xbase_delete(xbase: *mut Xbase) {
    let layout = Layout::new::<Xbase>();
    dealloc(xbase as *mut u8, layout);
}
