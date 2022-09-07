use std::os::raw::*;

#[repr(C)]
pub struct Position {
    x: c_int,
    y: c_int,
}

impl Default for Position {
    fn default() -> Self {
        Self::new(0, 0)
    }
}

impl Position {
    fn new(x: c_int, y: c_int) -> Self {
        Self { x, y }
    }

    fn x(&self) -> c_int {
        self.x
    }

    fn y(&self) -> c_int {
        self.y
    }
}
