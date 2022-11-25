#include "spawn.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS_COUNT 25
#define ARGS_SIZE (MAX_ARGS_COUNT + 1)
#define MON_ARG_SIZE 2

#ifdef WITH_LOCKER_I3LOCK_COLOR
#define COLOR_BLANK     "#00000000"
#define COLOR_CLEAR     "#ffffff22"
#define COLOR_DEFAULT   "#ff00ffcc"
#define COLOR_TEXT      "#ee00eeee"
#define COLOR_WRONG     "#880000bb"
#define COLOR_VERIFYING "#bb00bbbb"
#endif // WITH_LOCKER_I3LOCK_COLOR

struct Command {
	const char *name;
	size_t monitor_arg_index;
	const char *args[ARGS_SIZE];
};

static struct Command commands[] = {
#ifdef WITH_LOCKER
	{
		.name = "lock",
		.monitor_arg_index = 0,
#ifdef WITH_LOCKER_I3LOCK
		.args = { "i3lock", NULL },
#endif // WITH_LOCKER_I3LOCK
#ifdef WITH_LOCKER_I3LOCK_COLOR
		.args = {
			"i3lock",
			"--insidever-color="COLOR_CLEAR,
			"--ringver-color="COLOR_VERIFYING,

			"--insidewrong-color="COLOR_CLEAR,
			"--ringwrong-color="COLOR_WRONG,

			"--inside-color="COLOR_BLANK,
			"--ring-color="COLOR_DEFAULT,
			"--line-color="COLOR_BLANK,
			"--separator-color="COLOR_DEFAULT,

			"--verif-color="COLOR_TEXT,
			"--wrong-color="COLOR_TEXT,
			"--time-color="COLOR_TEXT,
			"--date-color="COLOR_TEXT,
			"--layout-color="COLOR_TEXT,
			"--keyhl-color="COLOR_WRONG,
			"--bshl-color="COLOR_WRONG,

			"--screen=1",
			"--blur=5",
			"--clock",
			"--indicator",
			"--time-str=%H:%M:%S",
			"--date-str=%a, %e %b %Y",
			"--keylayout=1",
			NULL,
		},
#endif // WITH_LOCKER_I3LOCK_COLOR
	},
#endif // WITH_LOCKER
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
#ifdef WITH_TERMINAL
	{
		.name = "term",
		.monitor_arg_index = 0,
#ifdef WITH_TERMINAL_ALACRITTY
		.args = { "alacritty", NULL },
#endif // WITH_TERMINAL_ALACRITTY
#ifdef WITH_TERMINAL_GNOME
		.args = { "gnome-terminal", "--wait", NULL },
#endif // WITH_TERMINAL_GNOME
#ifdef WITH_TERMINAL_ST
		.args = { "st", NULL },
#endif // WITH_TERMINAL_ST
#ifdef WITH_TERMINAL_XTERM
		.args = { "xterm", NULL },
#endif // WITH_TERMINAL_XTERM
	},
#endif // WITH_TERMINAL
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
