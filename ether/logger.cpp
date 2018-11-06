/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

static inline void __message(
	const char * type,
	const char * file,
	const char * func,
	int line,
	const char * msg)
{
	(void) file;
	(void) func;
	(void) line;
    fprintf(stdout, "[%s] - %s\n", type, msg);
}

void  Logger::Debug(const char * file, const char * func, int line, const char * format, ...)
{
	char msg[512];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(msg), format, ap);
	va_end(ap);
	__message("DBG", file, func, line, msg);
}

void Logger::Warning(const char * file, const char * func, int line, const char * format, ...)
{
	char msg[512];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(msg), format, ap);
	va_end(ap);
	__message("WRN", file, func, line, msg);
}

void Logger::Error(const char * file, const char * func, int line, const char * format, ...)
{
	char msg[512];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(msg), format, ap);
	va_end(ap);
	__message("ERR", file, func, line, msg);
}

void Logger::Fatal(const char * file, const char * func, int line, const char * format, ...)
{
	char msg[512];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(msg), format, ap);
	va_end(ap);
	__message("FAT", file, func, line, msg);
	exit(1);
}
