#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void logger(const char *level, const char *fmt, ...);
static void logger_perror(const char *level, const char *fmt, ...);

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
