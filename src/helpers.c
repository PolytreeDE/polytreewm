#include "helpers.h"

#include "settings.h"

int helpers_border_width(
	const unsigned int displayed_clients,
	const bool selected_is_fullscreen,
	const bool any_is_fullscreen
) {
	const SettingsForSingleWindow border_for_single_window = settings_get_border_for_single_window();
	const int border_width = settings_get_border_width();

	if (displayed_clients > 1) return border_width;

	switch (border_for_single_window) {
	case SETTINGS_FOR_SINGLE_WINDOW_NEVER:
		return 0;
	case SETTINGS_FOR_SINGLE_WINDOW_ALWAYS:
		return border_width;
	case SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN:
		return selected_is_fullscreen ? 0 : border_width;
	case SETTINGS_FOR_SINGLE_WINDOW_NOBODY_IS_FULLSCREEN:
		return (selected_is_fullscreen || any_is_fullscreen) ? 0 : border_width;
	default:
		// TODO: maybe we should assert here
		return 0;
	}
}
