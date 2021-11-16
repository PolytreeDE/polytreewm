#ifndef _LAYOUTS_H
#define _LAYOUTS_H

#include <stddef.h>

typedef void (*LayoutsSymbolFunc)(
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

void layouts_symbol_func(
	LayoutsSymbolFunc func,
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

void layouts_symbol_unknown(
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

void layouts_symbol_monocle(
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

void layouts_symbol_floating(
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

void layouts_symbol_tile(
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

void layouts_symbol_horizontile(
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

void layouts_symbol_centeredmaster(
	char *buffer,
	size_t buffer_size,
	unsigned int clients_in_master,
	unsigned int visible_clients
);

#endif // _LAYOUTS_H
