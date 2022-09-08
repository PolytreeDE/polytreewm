use crate::constraints;
use crate::unit;

use std::os::raw::*;

#[derive(Clone, Copy, Debug)]
pub struct Settings {
    bar_on_top_by_default: bool,
    border_for_single_window: ForSingleWindow,
    border_width: c_int,
    default_clients_in_master: c_int,
    default_master_area_factor: c_float,
    enable_swallowing: bool,
    focus_on_wheel: bool,
    gap_for_single_window: ForSingleWindow,
    gap_size: c_int,
    master_area_factor_per_unit: unit::Kind,
    max_clients_in_master: Option<c_int>,
    respect_resize_hints_in_floating_layout: bool,
    show_bar_by_default: bool,
    snap_distance: c_uint,
    swallow_floating: bool,
}

#[derive(Clone, Copy, Debug)]
pub enum ForSingleWindow {
    Never,
    Always,
    NotInFullscreen,
    NobodyIsFullscreen,
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            bar_on_top_by_default: true,
            border_for_single_window: Default::default(),
            border_width: 2,
            default_clients_in_master: 1,
            default_master_area_factor: 0.6,
            enable_swallowing: true,
            focus_on_wheel: true,
            gap_for_single_window: Default::default(),
            gap_size: 10,
            master_area_factor_per_unit: unit::Kind::Monitor,
            max_clients_in_master: None,
            respect_resize_hints_in_floating_layout: false,
            show_bar_by_default: true,
            snap_distance: 32,
            swallow_floating: false,
        }
    }
}

impl Default for ForSingleWindow {
    fn default() -> Self {
        Self::NobodyIsFullscreen
    }
}

impl Into<c_uchar> for ForSingleWindow {
    fn into(self) -> c_uchar {
        match self {
            Self::Never => 0,
            Self::Always => 1,
            Self::NotInFullscreen => 2,
            Self::NobodyIsFullscreen => 3,
        }
    }
}

impl From<c_uchar> for ForSingleWindow {
    fn from(value: c_uchar) -> Self {
        match value {
            0 => Self::Never,
            1 => Self::Always,
            2 => Self::NotInFullscreen,
            3 => Self::NobodyIsFullscreen,
            _ => panic!("invalid value for type ForSingleWindow"),
        }
    }
}

impl Settings {
    pub fn bar_on_top_by_default(&self) -> bool {
        self.bar_on_top_by_default
    }

    pub fn bar_on_top_by_default_set(&mut self, value: bool) {
        self.bar_on_top_by_default = value;
    }

    pub fn border_for_single_window(&self) -> ForSingleWindow {
        self.border_for_single_window
    }

    // TODO: notify WM to rearrange clients
    pub fn border_for_single_window_set(&mut self, value: ForSingleWindow) {
        self.border_for_single_window = value;
    }

    pub fn border_width(&self) -> c_int {
        self.border_width
    }

    // TODO: notify WM to rearrange clients
    pub fn border_width_set(&mut self, value: c_int) {
        self.border_width = constraints::border_width(value);
    }

    pub fn default_clients_in_master(&self) -> c_int {
        self.default_clients_in_master
    }

    pub fn default_clients_in_master_set(&mut self, value: c_int) {
        self.default_clients_in_master =
            constraints::default_clients_in_master(value);
    }

    pub fn default_master_area_factor(&self) -> c_float {
        self.default_master_area_factor
    }

    pub fn default_master_area_factor_set(&mut self, value: c_float) {
        self.default_master_area_factor =
            constraints::default_master_area_factor(value);
    }

    pub fn enable_swallowing(&self) -> bool {
        self.enable_swallowing
    }

    pub fn enable_swallowing_set(&mut self, value: bool) {
        self.enable_swallowing = value;
    }

    pub fn focus_on_wheel(&self) -> bool {
        self.focus_on_wheel
    }

    pub fn focus_on_wheel_set(&mut self, value: bool) {
        self.focus_on_wheel = value;
    }

    pub fn gap_for_single_window(&self) -> ForSingleWindow {
        self.gap_for_single_window
    }

    // TODO: notify WM to rearrange clients
    pub fn gap_for_single_window_set(&mut self, value: ForSingleWindow) {
        self.gap_for_single_window = value;
    }

    pub fn gap_size(&self) -> c_int {
        self.gap_size
    }

    // TODO: notify WM to rearrange clients
    pub fn gap_size_set(&mut self, value: c_int) {
        self.gap_size = constraints::gap_size(value);
    }

    pub fn master_area_factor_per_unit(&self) -> unit::Kind {
        self.master_area_factor_per_unit
    }

    pub fn master_area_factor_per_unit_set(&mut self, value: unit::Kind) {
        self.master_area_factor_per_unit = value;
    }

    pub fn max_clients_in_master(&self) -> Option<c_int> {
        self.max_clients_in_master
    }

    // TODO: notify WM to rearrange clients
    pub fn max_clients_in_master_set(&mut self, value: Option<c_int>) {
        self.max_clients_in_master =
            value.map(|value| constraints::max_clients_in_master(value));
    }

    pub fn respect_resize_hints_in_floating_layout(&self) -> bool {
        self.respect_resize_hints_in_floating_layout
    }

    // TODO: notify WM to rearrange clients
    pub fn respect_resize_hints_in_floating_layout_set(&mut self, value: bool) {
        self.respect_resize_hints_in_floating_layout = value;
    }

    pub fn show_bar_by_default(&self) -> bool {
        self.show_bar_by_default
    }

    pub fn show_bar_by_default_set(&mut self, value: bool) {
        self.show_bar_by_default = value;
    }

    pub fn snap_distance(&self) -> c_uint {
        self.snap_distance
    }

    pub fn snap_distance_set(&mut self, value: c_uint) {
        self.snap_distance = constraints::snap_distance(value);
    }

    pub fn swallow_floating(&self) -> bool {
        self.swallow_floating
    }

    pub fn swallow_floating_set(&mut self, value: bool) {
        self.swallow_floating = value;
    }
}
