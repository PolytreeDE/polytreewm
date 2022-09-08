use crate::constraints;

use std::os::raw::*;

#[derive(Clone, Copy, Debug)]
pub struct Settings {
    bar_on_top_by_default: bool,
    border_width: c_int,
    default_clients_in_master: c_int,
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            bar_on_top_by_default: true,
            border_width: 2,
            default_clients_in_master: 1,
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
}
