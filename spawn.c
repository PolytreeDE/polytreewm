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
		.name = "menu",
		.monitor_arg_index = 2,
		.args = {
			"dmenu_run",
			// monitor
			"-m",
			"0",
			// font
			"-fn",
			"monospace:size=10",
			// color: gray 1
			"-nb",
			"#222222",
			// color: gray 3
			"-nf",
			"#bbbbbb",
			// color: cyan
			"-sb",
			"#005577",
			// color: gray 4
			"-sf",
			"#eeeeee",
			NULL,
		},
	},
	{
		.name = "term",
		.monitor_arg_index = -1,
		.args = { "st", NULL },
	},
	{
		.name = "firefox",
		.monitor_arg_index = -1,
		.args = { "firefox", NULL },
	},
};

void spawn_command(
	const char *const name,
	void (*const callback)(),
	const int monitor
) {
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

		if (command->monitor_arg_index != -1) {
			args = malloc(sizeof(char*) * ARGS_SIZE);
			if (args == NULL) break;

			for (size_t arg_index = 0;; ++arg_index) {
				// We discard const modifier
				// because we will only change newly allocated version.
				args[arg_index] = (char*)command->args[arg_index];
				if (args[arg_index] == NULL) break;
			}

			args[command->monitor_arg_index] =
				malloc(sizeof(char) * MON_ARG_SIZE);
			if (args[command->monitor_arg_index] == NULL) {
				free(args);
				break;
			}

			args[command->monitor_arg_index][0] = '0' + monitor;
			args[command->monitor_arg_index][1] = '\0';
		}

		if (fork() == 0) {
			// TODO: DRY (see the same code below)
			if (command->monitor_arg_index != -1) {
				free(args[command->monitor_arg_index]);
				free(args);
			}

			callback();
			setsid();
			execvp(args[0], args);
			fprintf(stderr, "polytreewm: execvp %s", args[0]);
			perror(" failed");
			exit(EXIT_SUCCESS);
		}

		if (command->monitor_arg_index != -1) {
			free(args[command->monitor_arg_index]);
			free(args);
		}

		break;
	}
}
