#include "main.h"

#include "dwm.h"

#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc == 2 && strcmp(argv[1], "-v") == 0) {
		fputs("polytreewm-"VERSION"\n", stderr);
		exit(EXIT_SUCCESS);
	}

	if (argc != 1) {
		die("usage: polytreewm [-v]");
	}

	if (!setlocale(LC_CTYPE, "") || !dwm_has_locale_support()) {
		warning("no locale support");
	}

	return dwm_main(argc, argv);
}

void die(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}

void die_perror(const char *const fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, ": ");
	perror(NULL);

	exit(EXIT_FAILURE);
}

void warning(const char *const fmt, ...)
{
	fputs("WARN: ", stderr);

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fputc('\n', stderr);
}
