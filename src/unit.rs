use std::os::raw::*;

#[derive(Clone, Copy, Debug)]
pub enum Kind {
    Global,
    Monitor,
    Tag,
}

impl Into<c_uchar> for Kind {
    fn into(self) -> c_uchar {
        match self {
            Self::Global => 0,
            Self::Monitor => 1,
            Self::Tag => 2,
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
