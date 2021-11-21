#ifndef _MAIN_H
#define _MAIN_H

__attribute__((noreturn))
void fatal(const char *fmt, ...);
__attribute__((noreturn))
void fatal_perror(const char *fmt, ...);
void fatal_nodie(const char *fmt, ...);
void fatal_perror_nodie(const char *fmt, ...);
void warning(const char *fmt, ...);
void warning_perror(const char *fmt, ...);
void info(const char *fmt, ...);

__attribute__((noreturn))
void restart();

#endif // _MAIN_H
