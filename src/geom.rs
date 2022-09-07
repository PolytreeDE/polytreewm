use std::os::raw::*;

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct Position {
    x: c_int,
    y: c_int,
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct Sizes {
    width: c_int,
    height: c_int,
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct BasicGeom {
    position: Position,
    sizes: Sizes,
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct WinGeom {
    basic: BasicGeom,
    border_width: c_int,
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

impl BasicGeom {
    pub fn new(position: Position, sizes: Sizes) -> Self {
        Self { position, sizes }
    }

    pub fn position(&self) -> Position {
        self.position
    }

    pub fn sizes(&self) -> Sizes {
        self.sizes
    }
}

impl WinGeom {
    pub fn new(basic: BasicGeom, border_width: c_int) -> Self {
        Self {
            basic,
            border_width,
        }
    }

    pub fn basic(&self) -> BasicGeom {
        self.basic
    }

    pub fn border_width(&self) -> c_int {
        self.border_width
    }

    pub fn total_width(&self) -> c_int {
        self.basic.sizes.width + 2 * self.border_width
    }

    pub fn total_height(&self) -> c_int {
        self.basic.sizes.height + 2 * self.border_width
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

    #[test]
    fn win_geom_default() {
        assert_eq!(WinGeom::default().border_width(), 0);
    }

    #[test]
    fn win_geom_total_width_height() {
        let sizes = Sizes::new(34, 56);
        let basic_geom = BasicGeom::new(Default::default(), sizes);
        let win_geom = WinGeom::new(basic_geom, 12);
        assert_eq!(win_geom.total_width(), 58);
        assert_eq!(win_geom.total_height(), 80);
    }
}
