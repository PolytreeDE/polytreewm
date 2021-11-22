#include "main.h"

#include "dwm.h"

#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROGRAM_TITLE "PolytreeWM"
#define PROGRAM_NAME  "polytreewm"

static char *program_exe = NULL;

static void signal_callback(int signo);

static void logger(const char *level, const char *fmt, ...);
static void logger_perror(const char *level, const char *fmt, ...);

int main(int argc, char *argv[])
{
	if (argc == 2 &&
		(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0))
	{
		fputs(PROGRAM_NAME"-"VERSION"\n", stderr);
		exit(EXIT_SUCCESS);
	}

	if (argc != 1 ||
		(argc == 2 &&
			(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
	{
		fputs("usage: "PROGRAM_NAME" [-v]\n", stderr);
		exit(EXIT_FAILURE);
	}

	if (!(program_exe = argv[0])) {
		fatal("no program executable name");
	}

	if (!setlocale(LC_CTYPE, "")) {
		warning("no locale support");
	}

#ifdef __OpenBSD__
	if (pledge("stdio rpath proc exec", NULL) == -1) {
		fatal("pledge");
	}
#endif // __OpenBSD__

	{
		struct sigaction action;
		memset(&action, 0, sizeof(action));
		action.sa_handler = signal_callback;

		if (sigaction(SIGCHLD, &action, NULL) != 0) {
			fatal_perror("can't install SIGCHLD handler");
		}
	}

	exit(dwm_main(PROGRAM_TITLE));
}

void signal_callback(const int signo)
{
	switch (signo) {
	case SIGCHLD:
		// Clean up any zombies immediately.
		while (waitpid(-1, NULL, WNOHANG) > 0);
		break;
	}
}

void restart()
{
	info("restarting");
	char *args[] = { program_exe, NULL };
	execvp(program_exe, args);
	fatal_perror("restart with `execvp' failed");
}

void logger(const char *const level, const char *const fmt, ...)
{
	fprintf(stderr, PROGRAM_NAME": %s: ", level);

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fputc('\n', stderr);
}

void logger_perror(const char *const level, const char *const fmt, ...)
{
	fprintf(stderr, PROGRAM_NAME": %s: ", level);

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, ": ");
	perror(NULL);
}

void fatal(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	logger("ERROR", fmt, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}

void fatal_perror(const char* const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	logger_perror("ERROR", fmt, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}

void fatal_nodie(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	logger("ERROR", fmt, ap);
	va_end(ap);
}

void fatal_perror_nodie(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	logger_perror("ERROR", fmt, ap);
	va_end(ap);
}

void warning(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	logger("WARN", fmt, ap);
	va_end(ap);
}

void warning_perror(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	logger_perror("WARN", fmt, ap);
	va_end(ap);
}

void info(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	logger("INFO", fmt, ap);
	va_end(ap);
}
