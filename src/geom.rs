use std::os::raw::*;

#[repr(C)]
#[derive(Default)]
pub struct Position {
    x: c_int,
    y: c_int,
}

#[repr(C)]
#[derive(Default)]
pub struct Sizes {
    width: c_int,
    height: c_int,
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn position_default() {
        assert_eq!(Position::default().x(), 0);
        assert_eq!(Position::default().y(), 0);
    }

    #[test]
    fn sizes_default() {
        assert_eq!(Sizes::default().width(), 0);
        assert_eq!(Sizes::default().height(), 0);
    }
}
