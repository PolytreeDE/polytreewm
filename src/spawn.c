#include "spawn.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS_COUNT 20
#define ARGS_SIZE (MAX_ARGS_COUNT + 1)
#define MON_ARG_SIZE 2

struct Command {
	const char *name;
	size_t monitor_arg_index;
	const char *args[ARGS_SIZE];
};

static struct Command commands[] = {
	{
		.name = "lock",
		.monitor_arg_index = 0,
		.args = { "i3lock", NULL },
	},
	{
		.name = "menu",
		.monitor_arg_index = 6,
		.args = {
			"rofi",
			"-modi",
			"drun",
			"-show",
			"drun",
			"-monitor",
			"-1",
			"-show-icons",
			NULL,
		},
	},
	{
		.name = "term",
		.monitor_arg_index = 0,
#ifdef ENABLE_GNOME_TERMINAL
		.args = { "gnome-terminal", "--wait", NULL },
#else
		.args = { "ptterm", NULL },
#endif
	},
	{
		.name = "firefox",
		.monitor_arg_index = 0,
		.args = { "firefox", NULL },
	},
};

void spawn_command(
	const char *const name,
	void (*const callback)(),
	const int monitor
) {
	// TODO: maybe we should assert here
	if (name == NULL || monitor < 0 || monitor > 9) return;

	for (
		size_t command_index = 0;
		command_index < sizeof(commands) / sizeof(struct Command);
		++command_index
	) {
		const struct Command *const command = &commands[command_index];
		if (strcmp(name, command->name) != 0) continue;

		// We discard const modifier
		// because we will only change newly allocated version.
		char **args = (char**)command->args;
		char monitor_buffer[2] = { '0' + monitor, '\0' };

		if (command->monitor_arg_index != 0) {
			args = malloc(sizeof(char*) * ARGS_SIZE);
			if (args == NULL) return;

			for (size_t arg_index = 0;; ++arg_index) {
				// We discard const modifier
				// because we will only change newly allocated version.
				args[arg_index] = (char*)command->args[arg_index];
				if (args[arg_index] == NULL) break;
			}

			args[command->monitor_arg_index] = monitor_buffer;
		}

		if (fork() == 0) {
			callback();
			setsid();
			execvp(args[0], args);
			fprintf(stderr, "polytreewm: execvp %s", args[0]);
			perror(" failed");
			exit(EXIT_SUCCESS);
		}

		if (command->monitor_arg_index != 0) free(args);

		return;
	}
}
