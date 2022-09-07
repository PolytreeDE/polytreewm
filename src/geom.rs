use std::os::raw::*;

#[repr(C)]
pub struct Position {
    x: c_int,
    y: c_int,
}

#[repr(C)]
pub struct Sizes {
    width: c_int,
    height: c_int,
}

impl Default for Position {
    fn default() -> Self {
        Self::new(0, 0)
    }
}

impl Default for Sizes {
    fn default() -> Self {
        Self::new(0, 0)
    }
}

impl Position {
    pub fn new(x: c_int, y: c_int) -> Self {
        Self { x, y }
    }

    pub fn x(&self) -> c_int {
        self.x
    }

    pub fn y(&self) -> c_int {
        self.y
    }
}

impl Sizes {
    pub fn new(width: c_int, height: c_int) -> Self {
        Self { width, height }
    }

    pub fn width(&self) -> c_int {
        self.width
    }

    pub fn height(&self) -> c_int {
        self.height
    }
}
