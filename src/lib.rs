use std::os::raw::*;

const MIN_BORDER_WIDTH: c_int = 0;
const MAX_BORDER_WIDTH: c_int = 10000;

#[no_mangle]
pub extern "C" fn constraints_border_width(border_width: c_int) -> c_int {
	if border_width < MIN_BORDER_WIDTH { return MIN_BORDER_WIDTH }
	if border_width > MAX_BORDER_WIDTH { return MAX_BORDER_WIDTH }
	border_width
}
