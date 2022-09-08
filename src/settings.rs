use crate::constraints;

use std::os::raw::*;

#[derive(Clone, Copy, Debug)]
pub struct Settings {
    bar_on_top_by_default: bool,
    border_width: c_int,
    default_clients_in_master: c_int,
    default_master_area_factor: c_float,
    enable_swallowing: bool,
    focus_on_wheel: bool,
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            bar_on_top_by_default: true,
            border_width: 2,
            default_clients_in_master: 1,
            default_master_area_factor: 0.6,
            enable_swallowing: true,
            focus_on_wheel: true,
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
}
