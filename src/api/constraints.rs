use crate::*;

use std::os::raw::*;

#[no_mangle]
extern "C" fn constraints_master_area_factor(
    master_area_factor: c_float,
) -> c_float {
    constraints::master_area_factor(master_area_factor)
}

#[no_mangle]
extern "C" fn constraints_snap_distance(snap_distance: c_uint) -> c_uint {
    constraints::snap_distance(snap_distance)
}
