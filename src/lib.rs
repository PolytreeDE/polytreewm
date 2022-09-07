mod constraints;

use std::os::raw::*;

#[no_mangle]
extern "C" fn constraints_border_width(border_width: c_int) -> c_int {
	constraints::border_width(border_width)
}

#[no_mangle]
extern "C" fn constraints_default_clients_in_master(default_clients_in_master: c_int) -> c_int {
	constraints::default_clients_in_master(default_clients_in_master)
}
