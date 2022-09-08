use crate::*;

use std::os::raw::*;

#[no_mangle]
extern "C" fn constraints_gap_size(gap_size: c_int) -> c_int {
    constraints::gap_size(gap_size)
}

#[no_mangle]
extern "C" fn constraints_master_area_factor(
    master_area_factor: c_float,
) -> c_float {
    constraints::master_area_factor(master_area_factor)
}

#[no_mangle]
extern "C" fn constraints_max_clients_in_master(
    max_clients_in_master: c_int,
) -> c_int {
    constraints::max_clients_in_master(max_clients_in_master)
}

#[no_mangle]
extern "C" fn constraints_snap_distance(snap_distance: c_uint) -> c_uint {
    constraints::snap_distance(snap_distance)
}
