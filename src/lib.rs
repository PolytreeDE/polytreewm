mod constraints;
mod geom;

use std::os::raw::*;

/***************
 * Constraints *
 ***************/

#[no_mangle]
extern "C" fn constraints_border_width(border_width: c_int) -> c_int {
    constraints::border_width(border_width)
}

#[no_mangle]
extern "C" fn constraints_default_clients_in_master(
    default_clients_in_master: c_int,
) -> c_int {
    constraints::default_clients_in_master(default_clients_in_master)
}

#[no_mangle]
extern "C" fn constraints_default_master_area_factor(
    default_master_area_factor: c_float,
) -> c_float {
    constraints::default_master_area_factor(default_master_area_factor)
}

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

/********
 * Geom *
 ********/

#[no_mangle]
unsafe extern "C" fn position_init(position: *mut geom::Position) {
    *position = Default::default();
}
