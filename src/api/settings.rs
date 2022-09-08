use crate::*;

use ctor::ctor;

static mut SETTINGS: Option<Settings> = None;

#[ctor]
unsafe fn ctor() {
    SETTINGS = Some(Default::default());
}

#[no_mangle]
unsafe extern "C" fn settings_get_bar_on_top_by_default() -> bool {
    SETTINGS.unwrap().bar_on_top_by_default()
}

#[no_mangle]
unsafe extern "C" fn settings_set_bar_on_top_by_default(value: bool) {
    SETTINGS.unwrap().bar_on_top_by_default_set(value);
}
