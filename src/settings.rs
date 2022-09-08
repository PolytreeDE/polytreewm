use crate::constraints;
use crate::unit;

use std::os::raw::*;

#[derive(Clone, Copy, Debug)]
pub struct Settings {
    bar_on_top_by_default: bool,
    border: Border,
    default_clients_in_master: c_int,
    default_master_area_factor: c_float,
    enable_swallowing: bool,
    focus_on_wheel: bool,
    gap: Gap,
    master_area_factor_per_unit: unit::Kind,
    max_clients_in_master: Option<c_int>,
    respect_resize_hints_in_floating_layout: bool,
    show_bar_by_default: bool,
    show_bar_per_unit: unit::Kind,
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

#[derive(Clone, Copy, Debug)]
pub struct Border {
    mode: ForSingleWindow,
    width: c_int,
}

#[derive(Clone, Copy, Debug)]
pub struct Gap {
    mode: ForSingleWindow,
    size: c_int,
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            bar_on_top_by_default: true,
            border: Default::default(),
            default_clients_in_master: 1,
            default_master_area_factor: 0.6,
            enable_swallowing: true,
            focus_on_wheel: true,
            gap: Default::default(),
            master_area_factor_per_unit: unit::Kind::Monitor,
            max_clients_in_master: None,
            respect_resize_hints_in_floating_layout: false,
            show_bar_by_default: true,
            show_bar_per_unit: unit::Kind::Monitor,
            snap_distance: 32,
            swallow_floating: false,
        }
    }
}

impl Default for Border {
    fn default() -> Self {
        Self {
            mode: Default::default(),
            width: 2,
        }
    }
}

impl Default for Gap {
    fn default() -> Self {
        Self {
            mode: Default::default(),
            size: 10,
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

    pub fn border(&mut self) -> &mut Border {
        &mut self.border
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

    pub fn gap(&mut self) -> &mut Gap {
        &mut self.gap
    }

    pub fn master_area_factor_per_unit(&self) -> unit::Kind {
        self.master_area_factor_per_unit
    }

    // TODO: notify WM to rearrange clients
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

    pub fn show_bar_per_unit(&self) -> unit::Kind {
        self.show_bar_per_unit
    }

    // TODO: notify WM to rearrange clients
    pub fn show_bar_per_unit_set(&mut self, value: unit::Kind) {
        self.show_bar_per_unit = value;
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

impl Border {
    pub fn mode(&self) -> ForSingleWindow {
        self.mode
    }

    pub fn mode_set(&mut self, value: ForSingleWindow) {
        self.mode = value;
    }

    pub fn width(&self) -> c_int {
        self.width
    }

    pub fn width_set(&mut self, value: c_int) {
        self.width = constraints::border_width(value);
    }

    pub fn width_helper(
        &self,
        displayed_clients: c_uint,
        selected_is_fullscreen: bool,
        any_is_fullscreen: bool,
    ) -> c_int {
        if displayed_clients > 1 {
            return self.width;
        }

        match self.mode {
            ForSingleWindow::Never => 0,
            ForSingleWindow::Always => self.width,
            ForSingleWindow::NotInFullscreen => {
                if selected_is_fullscreen {
                    0
                } else {
                    self.width
                }
            }
            ForSingleWindow::NobodyIsFullscreen => {
                if selected_is_fullscreen || any_is_fullscreen {
                    0
                } else {
                    self.width
                }
            }
        }
    }
}

impl Gap {
    pub fn mode(&self) -> ForSingleWindow {
        self.mode
    }

    pub fn mode_set(&mut self, value: ForSingleWindow) {
        self.mode = value;
    }

    pub fn size(&self) -> c_int {
        self.size
    }

    pub fn size_set(&mut self, value: c_int) {
        self.size = constraints::gap_size(value);
    }

    pub fn size_helper(
        &self,
        displayed_clients: c_uint,
        selected_is_fullscreen: bool,
        any_is_fullscreen: bool,
    ) -> c_int {
        if displayed_clients > 1 {
            return self.size;
        }

        match self.mode {
            ForSingleWindow::Never => 0,
            ForSingleWindow::Always => self.size,
            ForSingleWindow::NotInFullscreen => {
                if selected_is_fullscreen {
                    0
                } else {
                    self.size
                }
            }
            ForSingleWindow::NobodyIsFullscreen => {
                if selected_is_fullscreen || any_is_fullscreen {
                    0
                } else {
                    self.size
                }
            }
        }
    }
}
