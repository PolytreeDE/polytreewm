#include "helpers.h"

#include "settings.h"

int helpers_gap_size(
	const unsigned int displayed_clients,
	const bool selected_is_fullscreen
) {
	const SettingsForSingleWindow gap_for_single_window = settings_get_gap_for_single_window();
	const int gap_size = settings_get_gap_size();

	if (displayed_clients > 1) return gap_size;

	switch (gap_for_single_window) {
	case SETTINGS_FOR_SINGLE_WINDOW_NEVER:
		return 0;
	case SETTINGS_FOR_SINGLE_WINDOW_ALWAYS:
		return gap_size;
	case SETTINGS_FOR_SINGLE_WINDOW_NOT_IN_FULLSCREEN:
		return selected_is_fullscreen ? 0 : gap_size;
	default:
		// TODO: maybe we should assert here
		return 0;
	}
}

int helpers_border_width(
	const unsigned int displayed_clients,
	const bool selected_is_fullscreen
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
	default:
		// TODO: maybe we should assert here
		return 0;
	}
}
