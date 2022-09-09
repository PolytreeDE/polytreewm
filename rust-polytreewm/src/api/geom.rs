use crate::*;

use std::os::raw::*;

#[no_mangle]
unsafe extern "C" fn position_init(position: *mut geom::Position) {
    *position = Default::default();
}

#[no_mangle]
unsafe extern "C" fn sizes_init(sizes: &mut geom::Sizes) {
    *sizes = Default::default();
}

#[no_mangle]
unsafe extern "C" fn basic_geom_init(basic_geom: &mut geom::BasicGeom) {
    *basic_geom = Default::default();
}

#[no_mangle]
unsafe extern "C" fn win_geom_init(win_geom: &mut geom::WinGeom) {
    *win_geom = Default::default();
}

#[no_mangle]
unsafe extern "C" fn position_init_from_args(
    position: *mut geom::Position,
    x: c_int,
    y: c_int,
) {
    *position = geom::Position::new(x, y);
}

#[no_mangle]
unsafe extern "C" fn sizes_init_from_args(
    sizes: *mut geom::Sizes,
    width: c_int,
    height: c_int,
) {
    *sizes = geom::Sizes::new(width, height);
}

#[no_mangle]
unsafe extern "C" fn basic_geom_init_from_args(
    basic_geom: *mut geom::BasicGeom,
    x: c_int,
    y: c_int,
    width: c_int,
    height: c_int,
) {
    *basic_geom = geom::BasicGeom::new(
        geom::Position::new(x, y),
        geom::Sizes::new(width, height),
    );
}

#[no_mangle]
unsafe extern "C" fn win_geom_init_from_args(
    win_geom: *mut geom::WinGeom,
    x: c_int,
    y: c_int,
    width: c_int,
    height: c_int,
    border_width: c_int,
) {
    *win_geom = geom::WinGeom::new(
        geom::BasicGeom::new(
            geom::Position::new(x, y),
            geom::Sizes::new(width, height),
        ),
        border_width,
    );
}

#[no_mangle]
unsafe extern "C" fn win_geom_total_width(
    win_geom: *const geom::WinGeom,
) -> c_int {
    (*win_geom).total_sizes().width()
}

#[no_mangle]
unsafe extern "C" fn win_geom_total_height(
    win_geom: *const geom::WinGeom,
) -> c_int {
    (*win_geom).total_sizes().height()
}
