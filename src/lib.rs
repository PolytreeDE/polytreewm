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

#[no_mangle]
extern "C" fn constraints_gap_size(gap_size: c_int) -> c_int {
	constraints::gap_size(gap_size)
}

#[no_mangle]
extern "C" fn constraints_max_clients_in_master(max_clients_in_master: c_int) -> c_int {
	constraints::max_clients_in_master(max_clients_in_master)
}
