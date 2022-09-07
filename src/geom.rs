use std::os::raw::*;

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub struct Position {
    x: c_int,
    y: c_int,
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub struct Sizes {
    width: c_int,
    height: c_int,
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub struct BasicGeom {
    position: Position,
    sizes: Sizes,
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
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

    pub fn total_position(&self) -> Position {
        Position::new(
            self.basic.position.x - self.border_width,
            self.basic.position.y - self.border_width,
        )
    }

    pub fn total_sizes(&self) -> Sizes {
        Sizes::new(
            self.basic.sizes.width + 2 * self.border_width,
            self.basic.sizes.height + 2 * self.border_width,
        )
    }

    pub fn total_geom(&self) -> BasicGeom {
        BasicGeom::new(self.total_position(), self.total_sizes())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn position_eq() {
        assert_eq!(Position::new(123, 456), Position::new(123, 456));

        assert_ne!(Position::new(123, 123), Position::new(123, 456));
        assert_ne!(Position::new(123, 456), Position::new(123, 123));

        assert_ne!(Position::new(123, 123), Position::new(456, 123));
        assert_ne!(Position::new(456, 123), Position::new(123, 123));

        assert_ne!(Position::new(123, 123), Position::new(456, 456));
        assert_ne!(Position::new(456, 456), Position::new(123, 123));
    }

    #[test]
    fn sizes_eq() {
        assert_eq!(Sizes::new(123, 456), Sizes::new(123, 456));

        assert_ne!(Sizes::new(123, 123), Sizes::new(123, 456));
        assert_ne!(Sizes::new(123, 456), Sizes::new(123, 123));

        assert_ne!(Sizes::new(123, 123), Sizes::new(456, 123));
        assert_ne!(Sizes::new(456, 123), Sizes::new(123, 123));

        assert_ne!(Sizes::new(123, 123), Sizes::new(456, 456));
        assert_ne!(Sizes::new(456, 456), Sizes::new(123, 123));
    }

    #[test]
    fn basic_geom_eq() {
        let pos1 = Position::new(123, 123);
        let pos2 = Position::new(123, 456);

        let sizes1 = Sizes::new(123, 123);
        let sizes2 = Sizes::new(123, 456);

        assert_eq!(BasicGeom::new(pos1, sizes1), BasicGeom::new(pos1, sizes1));
        assert_eq!(BasicGeom::new(pos1, sizes2), BasicGeom::new(pos1, sizes2));
        assert_eq!(BasicGeom::new(pos2, sizes1), BasicGeom::new(pos2, sizes1));
        assert_eq!(BasicGeom::new(pos2, sizes2), BasicGeom::new(pos2, sizes2));

        assert_ne!(BasicGeom::new(pos1, sizes1), BasicGeom::new(pos1, sizes2));
        assert_ne!(BasicGeom::new(pos1, sizes2), BasicGeom::new(pos1, sizes1));
        assert_ne!(BasicGeom::new(pos1, sizes1), BasicGeom::new(pos2, sizes1));
        assert_ne!(BasicGeom::new(pos2, sizes2), BasicGeom::new(pos1, sizes2));
    }

    #[test]
    fn win_geom_eq() {
        let basic_geom1 =
            BasicGeom::new(Position::new(123, 123), Sizes::new(123, 123));
        let basic_geom2 =
            BasicGeom::new(Position::new(123, 123), Sizes::new(123, 456));

        assert_eq!(
            WinGeom::new(basic_geom1, 99),
            WinGeom::new(basic_geom1, 99),
        );

        assert_ne!(
            WinGeom::new(basic_geom1, 55),
            WinGeom::new(basic_geom1, 99),
        );
        assert_ne!(
            WinGeom::new(basic_geom1, 99),
            WinGeom::new(basic_geom1, 55),
        );
        assert_ne!(
            WinGeom::new(basic_geom1, 99),
            WinGeom::new(basic_geom2, 99),
        );
        assert_ne!(
            WinGeom::new(basic_geom2, 99),
            WinGeom::new(basic_geom1, 99),
        );
    }

    #[test]
    fn position_default() {
        assert_eq!(Position::default(), Position::new(0, 0));
    }

    #[test]
    fn sizes_default() {
        assert_eq!(Sizes::default(), Sizes::new(0, 0));
    }

    #[test]
    fn win_geom_default() {
        assert_eq!(WinGeom::default(), WinGeom::new(Default::default(), 0));
    }

    #[test]
    fn win_geom_total_position() {
        let position = Position::new(34, 56);
        let basic_geom = BasicGeom::new(position, Default::default());
        let win_geom = WinGeom::new(basic_geom, 12);
        assert_eq!(win_geom.total_position(), Position::new(22, 44));
    }

    #[test]
    fn win_geom_total_sizes() {
        let sizes = Sizes::new(34, 56);
        let basic_geom = BasicGeom::new(Default::default(), sizes);
        let win_geom = WinGeom::new(basic_geom, 12);
        assert_eq!(win_geom.total_sizes(), Sizes::new(58, 80));
    }

    #[test]
    fn win_geom_total_geom() {
        let position = Position::new(34, 56);
        let sizes = Sizes::new(34, 56);
        let basic_geom = BasicGeom::new(position, sizes);
        let win_geom = WinGeom::new(basic_geom, 12);
        assert_eq!(
            win_geom.total_geom(),
            BasicGeom::new(Position::new(22, 44), Sizes::new(58, 80)),
        );
    }
}
