#include "menu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define COMMAND_SIZE 256

bool menu_run(
	char *const buffer,
	const size_t buffer_size,
	const char *prompt
) {
	// TODO: maybe we should assert here
	if (buffer == NULL || buffer_size < 2) return false;

	if (prompt == NULL) {
		prompt = "";
	}

	for (const char *s = prompt; *s; ++s) {
		// Simple check to ensure that the prompt
		// string doesn't need to be escaped.
		// TODO: maybe we should assert here
		if (*s == '"') return false;
	}

	char command[COMMAND_SIZE];
	const int command_slen =
		snprintf(command, COMMAND_SIZE, "rofi -dmenu -p \"%s\"", prompt);

	// Simple check to ensure that the prompt string is not too long.
	// TODO: maybe we should assert here
	if (command_slen >= COMMAND_SIZE) return false;

	errno = 0; // popen(3p) says on failure it "may" set errno
	FILE *f = popen(command, "r");

	if (!f) {
		perror("polytreewm: menu: popen failed");
		return false;
	}

	const char *const buffer_result = fgets(buffer, buffer_size, f);

	// Just to ensure that string is null-terminated.
	// The man page fgets(3) is not very clear about that.
	buffer[buffer_size - 1] = '\0';

	if (!buffer_result) {
		perror("polytreewm: menu: fgets failed");
		pclose(f);
		return false;
	}

	// Fail on both pipe error and not successfull exit status.
	if (pclose(f) != EXIT_SUCCESS) {
		perror("polytreewm: menu: pclose failed");
		return false;
	}

	for (char *s = buffer; *s; ++s) {
		if (*s == '\n') {
			if (*(s + 1) == '\0') {
				*s = '\0';
			} else {
				*s = ' ';
			}
		}
	}

	return true;
}
