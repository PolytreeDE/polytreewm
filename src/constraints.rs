use std::os::raw::*;

const MAX_BORDER_WIDTH: c_int = 10000;

pub fn border_width(border_width: c_int) -> c_int {
	if border_width > MAX_BORDER_WIDTH { return MAX_BORDER_WIDTH }
	border_width
}

#[cfg(test)]
mod tests {
	use super::*;

	#[test]
	fn test_border_width() {
		assert_eq!(border_width(0), 0);
		assert_eq!(border_width(10), 10);
		assert_eq!(border_width(10_000), 10_000);

		assert_eq!(border_width(10_001), 10_000);
		assert_eq!(border_width(20_000), 10_000);
	}
}
