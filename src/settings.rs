#[derive(Clone, Copy, Debug)]
pub struct Settings {
    bar_on_top_by_default: bool,
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            bar_on_top_by_default: true,
        }
    }
}

impl Settings {
    pub fn bar_on_top_by_default(&self) -> bool {
        self.bar_on_top_by_default
    }

    pub fn bar_on_top_by_default_set(&mut self, bar_on_top_by_default: bool) {
        self.bar_on_top_by_default = bar_on_top_by_default;
    }
}
