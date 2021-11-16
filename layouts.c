#include "layouts.h"

#include <stdio.h>
#include <string.h>

#define TMP_BUFFER_SIZE 128

void layouts_symbol_func(
	LayoutsSymbolFunc func,
	char *const buffer,
	const size_t buffer_size,
	const unsigned int clients_in_master,
	const unsigned int visible_clients
) {
	if (func == NULL) {
		func = layouts_symbol_unknown;
	}

	func(
		buffer,
		buffer_size,
		clients_in_master,
		visible_clients
	);
}

void layouts_symbol_unknown(
	char *const buffer,
	const size_t buffer_size,
	const unsigned int clients_in_master,
	const unsigned int visible_clients
) {
	// TODO: maybe we should assert
	if (buffer == NULL) return;

	char tmp[TMP_BUFFER_SIZE];
	sprintf(tmp, "?%u/%u?", clients_in_master, visible_clients);
	tmp[(buffer_size > TMP_BUFFER_SIZE ? TMP_BUFFER_SIZE : buffer_size) - 1] = '\0';
	strncpy(buffer, tmp, buffer_size);
}

void layouts_symbol_monocle(
	char *const buffer,
	const size_t buffer_size,
	const unsigned int clients_in_master,
	const unsigned int visible_clients
) {
	// TODO: maybe we should assert
	if (buffer == NULL) return;

	char tmp[TMP_BUFFER_SIZE];
	sprintf(tmp, "[%u/%u]", clients_in_master, visible_clients);
	tmp[(buffer_size > TMP_BUFFER_SIZE ? TMP_BUFFER_SIZE : buffer_size) - 1] = '\0';
	strncpy(buffer, tmp, buffer_size);
}

void layouts_symbol_floating(
	char *const buffer,
	const size_t buffer_size,
	const unsigned int clients_in_master,
	const unsigned int visible_clients
) {
	// TODO: maybe we should assert
	if (buffer == NULL) return;

	char tmp[TMP_BUFFER_SIZE];
	sprintf(tmp, "<%u/%u>", clients_in_master, visible_clients);
	tmp[(buffer_size > TMP_BUFFER_SIZE ? TMP_BUFFER_SIZE : buffer_size) - 1] = '\0';
	strncpy(buffer, tmp, buffer_size);
}

void layouts_symbol_tile(
	char *const buffer,
	const size_t buffer_size,
	const unsigned int clients_in_master,
	const unsigned int visible_clients
) {
	// TODO: maybe we should assert
	if (buffer == NULL) return;

	char tmp[TMP_BUFFER_SIZE];
	sprintf(tmp, "{%u/%u}=", clients_in_master, visible_clients);
	tmp[(buffer_size > TMP_BUFFER_SIZE ? TMP_BUFFER_SIZE : buffer_size) - 1] = '\0';
	strncpy(buffer, tmp, buffer_size);
}

void layouts_symbol_horizontile(
	char *const buffer,
	const size_t buffer_size,
	const unsigned int clients_in_master,
	const unsigned int visible_clients
) {
	// TODO: maybe we should assert
	if (buffer == NULL) return;

	char tmp[TMP_BUFFER_SIZE];
	sprintf(tmp, "v{%u/%u}v", clients_in_master, visible_clients);
	tmp[(buffer_size > TMP_BUFFER_SIZE ? TMP_BUFFER_SIZE : buffer_size) - 1] = '\0';
	strncpy(buffer, tmp, buffer_size);
}

void layouts_symbol_centeredmaster(
	char *const buffer,
	const size_t buffer_size,
	const unsigned int clients_in_master,
	const unsigned int visible_clients
) {
	// TODO: maybe we should assert
	if (buffer == NULL) return;

	char tmp[TMP_BUFFER_SIZE];
	sprintf(tmp, "={%u/%u}=", clients_in_master, visible_clients);
	tmp[(buffer_size > TMP_BUFFER_SIZE ? TMP_BUFFER_SIZE : buffer_size) - 1] = '\0';
	strncpy(buffer, tmp, buffer_size);
}
