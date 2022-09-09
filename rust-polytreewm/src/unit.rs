use std::os::raw::*;

#[derive(Clone, Copy, Debug)]
pub enum Kind {
    Global,
    Monitor,
    Tag,
}

impl From<Kind> for c_uchar {
    fn from(value: Kind) -> Self {
        match value {
            Kind::Global => 0,
            Kind::Monitor => 1,
            Kind::Tag => 2,
        }
    }
}

impl From<c_uchar> for Kind {
    fn from(value: c_uchar) -> Self {
        match value {
            0 => Self::Global,
            1 => Self::Monitor,
            2 => Self::Tag,
            _ => panic!("invalid value for type Kind"),
        }
    }
}
