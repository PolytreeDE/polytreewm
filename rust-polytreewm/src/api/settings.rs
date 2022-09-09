use crate::*;

use std::os::raw::*;

use ctor::ctor;

static mut SETTINGS: Option<Settings> = None;

#[ctor]
unsafe fn ctor() {
    SETTINGS = Some(Default::default());
}

#[no_mangle]
unsafe extern "C" fn settings_get_bar_on_top_by_default() -> bool {
    SETTINGS.unwrap().bar_on_top_by_default()
}

#[no_mangle]
unsafe extern "C" fn settings_set_bar_on_top_by_default(value: bool) {
    SETTINGS.unwrap().bar_on_top_by_default_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_set_border_for_single_window(value: c_uchar) {
    SETTINGS.unwrap().border_mut().mode_set(value.into());
}

#[no_mangle]
unsafe extern "C" fn settings_get_border_width() -> c_int {
    SETTINGS.unwrap().border().width()
}

#[no_mangle]
unsafe extern "C" fn settings_set_border_width(value: c_int) {
    SETTINGS.unwrap().border_mut().width_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_default_clients_in_master() -> c_int {
    SETTINGS.unwrap().default_clients_in_master()
}

#[no_mangle]
unsafe extern "C" fn settings_set_default_clients_in_master(value: c_int) {
    SETTINGS.unwrap().default_clients_in_master_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_default_master_area_factor() -> c_float {
    SETTINGS.unwrap().default_master_area_factor()
}

#[no_mangle]
unsafe extern "C" fn settings_set_default_master_area_factor(value: c_float) {
    SETTINGS.unwrap().default_master_area_factor_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_enable_swallowing() -> bool {
    SETTINGS.unwrap().enable_swallowing()
}

#[no_mangle]
unsafe extern "C" fn settings_set_enable_swallowing(value: bool) {
    SETTINGS.unwrap().enable_swallowing_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_focus_on_wheel() -> bool {
    SETTINGS.unwrap().focus_on_wheel()
}

#[no_mangle]
unsafe extern "C" fn settings_set_focus_on_wheel(value: bool) {
    SETTINGS.unwrap().focus_on_wheel_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_set_gap_for_single_window(value: c_uchar) {
    SETTINGS.unwrap().gap_mut().mode_set(value.into());
}

#[no_mangle]
unsafe extern "C" fn settings_get_gap_size() -> c_int {
    SETTINGS.unwrap().gap().size()
}

#[no_mangle]
unsafe extern "C" fn settings_set_gap_size(value: c_int) {
    SETTINGS.unwrap().gap_mut().size_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_master_area_factor_per_unit() -> c_uchar {
    SETTINGS.unwrap().master_area_factor_per_unit().into()
}

#[no_mangle]
unsafe extern "C" fn settings_set_master_area_factor_per_unit(value: c_uchar) {
    SETTINGS
        .unwrap()
        .master_area_factor_per_unit_set(value.into());
}

#[no_mangle]
unsafe extern "C" fn settings_get_max_clients_in_master() -> c_int {
    SETTINGS.unwrap().max_clients_in_master().unwrap_or(0)
}

#[no_mangle]
unsafe extern "C" fn settings_set_max_clients_in_master(value: c_int) {
    SETTINGS.unwrap().max_clients_in_master_set(if value == 0 {
        None
    } else {
        Some(value)
    });
}

#[no_mangle]
unsafe extern "C" fn settings_get_respect_resize_hints_in_floating_layout(
) -> bool {
    SETTINGS.unwrap().respect_resize_hints_in_floating_layout()
}

#[no_mangle]
unsafe extern "C" fn settings_set_respect_resize_hints_in_floating_layout(
    value: bool,
) {
    SETTINGS
        .unwrap()
        .respect_resize_hints_in_floating_layout_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_show_bar_by_default() -> bool {
    SETTINGS.unwrap().show_bar_by_default()
}

#[no_mangle]
unsafe extern "C" fn settings_set_show_bar_by_default(value: bool) {
    SETTINGS.unwrap().show_bar_by_default_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_show_bar_per_unit() -> c_uchar {
    SETTINGS.unwrap().show_bar_per_unit().into()
}

#[no_mangle]
unsafe extern "C" fn settings_set_show_bar_per_unit(value: c_uchar) {
    SETTINGS.unwrap().show_bar_per_unit_set(value.into());
}

#[no_mangle]
unsafe extern "C" fn settings_get_snap_distance() -> c_uint {
    SETTINGS.unwrap().snap_distance()
}

#[no_mangle]
unsafe extern "C" fn settings_set_snap_distance(value: c_uint) {
    SETTINGS.unwrap().snap_distance_set(value);
}

#[no_mangle]
unsafe extern "C" fn settings_get_swallow_floatting() -> bool {
    SETTINGS.unwrap().swallow_floating()
}

#[no_mangle]
unsafe extern "C" fn settings_set_swallow_floating(value: bool) {
    SETTINGS.unwrap().swallow_floating_set(value);
}

#[no_mangle]
unsafe extern "C" fn helpers_border_width(
    displayed_clients: c_uint,
    selected_is_fullscreen: bool,
    any_is_fullscreen: bool,
) -> c_int {
    SETTINGS.unwrap().border().width_helper(
        displayed_clients,
        selected_is_fullscreen,
        any_is_fullscreen,
    )
}

#[no_mangle]
unsafe extern "C" fn helpers_gap_size(
    displayed_clients: c_uint,
    selected_is_fullscreen: bool,
    any_is_fullscreen: bool,
) -> c_int {
    SETTINGS.unwrap().gap().size_helper(
        displayed_clients,
        selected_is_fullscreen,
        any_is_fullscreen,
    )
}
