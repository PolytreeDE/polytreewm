use crate::*;

use std::ffi::{CStr, CString};
use std::os::raw::*;

#[no_mangle]
unsafe extern "C" fn spawn_command(
    name: *const c_char,
    callback: unsafe extern "C" fn(),
    monitor: c_int,
) {
    if name.is_null() || !(0..=9).contains(&monitor) {
        return;
    }
    let name = CStr::from_ptr(name).to_str().unwrap();
    let command = spawn::command(name);
    let mut args: Vec<String> =
        command.args.iter().map(|arg| arg.to_string()).collect();
    if command.monitor_arg_index != 0 {
        args[command.monitor_arg_index] = format!("{}", monitor);
    }
    let arg_cstrs: Vec<CString> = args
        .iter()
        .map(|arg| CString::new(arg.as_str()).unwrap())
        .collect();
    let arg_ptrs: Vec<*const c_char> =
        arg_cstrs.iter().map(|arg| arg.as_ptr()).collect();

    if libc::fork() != 0 {
        callback();
        libc::setsid();
        libc::execvp(arg_ptrs[0], arg_ptrs.as_ptr());
        libc::exit(libc::EXIT_SUCCESS);
    }
}
