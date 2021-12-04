#ifndef _LOGGER_H
#define _LOGGER_H

#define PROGRAM_TITLE "PolytreeWM"
#define PROGRAM_NAME  "polytreewm"

__attribute__((noreturn))
void fatal(const char *fmt, ...);
__attribute__((noreturn))
void fatal_perror(const char *fmt, ...);
void fatal_nodie(const char *fmt, ...);
void fatal_perror_nodie(const char *fmt, ...);
void warning(const char *fmt, ...);
void warning_perror(const char *fmt, ...);
void info(const char *fmt, ...);

#endif // _LOGGER_H
