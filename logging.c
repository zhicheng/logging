#include "logging.h"

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

char *
level_table[] = {
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */
	"NOTSET",	/* 0 ~ 9 */

	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */
	"DEBUG",	/* 10 ~ 19 */

	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */
	"INFO",		/* 20 ~ 29 */

	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */
	"WARNING",	/* 30 ~ 39 */

	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */
	"ERROR",	/* 40 ~ 49 */

	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
	"CRITICAL",	/* 50 ~ 59 */
};

static logger_t *loggers = NULL;

logger_t *
logging_getlogger(char name[])
{
	logger_t *logger;

	for (logger = loggers; logger != NULL; logger = logger->next) {
		if (strcmp(logger->name, name) == 0)
			break;
	}

	return logger;
}

void
logging_setlogger(logger_t *logger)
{
	logger_t **next;

	memcpy(logger->level_table, level_table, sizeof(level_table));
	for (next = &loggers; *next != NULL; next = &(*next)->next) {
		if (strcmp((*next)->name, logger->name) == 0)
			break;
	}

	*next = logger;
}

char *
logger_getlevelname(logger_t *logger, int level)
{
	assert(level < LOGGING_MAX_LEVEL);

	return logger->level_table[level];
}

void
logger_setlevelname(logger_t *logger, int level, char *name)
{
	assert(level < LOGGING_MAX_LEVEL);

	logger->level_table[level] = name;
}

/* 
 * 0 not log
 * 1 log
 */
static int
filter_filter(filter_t *filter, record_t *record)
{
	if (record->level < filter->minlevel)
		return 0;
	if (record->level > filter->maxlevel)
		return 0;
	return 1;
}

static void
formatter_format(formatter_t *formatter, record_t *record, char *out)
{
	int i;
	int len;
	int skip;
	int fmtlen;

	char *fmt;

	fmt = formatter->fmt;
	fmtlen = strlen(formatter->fmt);

	for (i = 0; i < fmtlen; i += skip) {
		if (memcmp(fmt + i, "%%", 2) == 0) {
			*out++ = '%';
			skip = 2;
		} else if (memcmp(fmt + i, "%(name)s", 8) == 0) {
			len = strlen(record->name);
			memcpy(out, record->name, len);
			out += len;
			skip = 8;
		} else if (memcmp(fmt + i, "%(levelno)d", 11) == 0) {
			len = sprintf(out, "%d", record->level);
			out += len;
			skip = 11;
		} else if (memcmp(fmt + i, "%(levelname)s", 13) == 0) {
			len = strlen(record->levelname);
			memcpy(out, record->levelname, len);
			out += len;
			skip = 13;
		} else if (memcmp(fmt + i, "%(message)s", 11) == 0) {
			len = strlen(record->message);
			memcpy(out, record->message, len);
			out += len;
			skip = 11;
		} else if (memcmp(fmt + i, "%(created)d", 11) == 0) {
			len = sprintf(out, "%ld", record->seconds);
			out += len;
			skip = 11;
		} else if (memcmp(fmt + i, "%(asctime)s", 11) == 0) {
			struct tm tm;
			gmtime_r(&record->seconds, &tm);
			len = strftime(out, 4096, formatter->datefmt, &tm);
			out += len;
			skip = 11;
		} else if (memcmp(fmt + i, "%(msecs)d", 9) == 0) {
			len = sprintf(out, "%d", record->useconds/1000);
			out += len;
			skip = 9;
		} else if (memcmp(fmt + i, "%(usecs)d", 9) == 0) {
			len = sprintf(out, "%d", record->useconds);
			out += len;
			skip = 9;
		} else {
			*out++ = fmt[i];
			skip = 1;
		}
	}
	*out = '\0';
}

static void
handler_emit(handler_t *handler, record_t *record)
{
	char buf[4096];
	filter_t *filter;

	for (filter = handler->filter; filter != NULL; filter = filter->next) {
		if (filter_filter(filter, record) == 0)
			return;
	}

	formatter_format(handler->formatter, record, buf);
	printf("%s: %s\n", handler->name, buf);
}

static void
log(logger_t *logger, int level, char *msg, va_list args)
{
	record_t   record;
	filter_t  *filter;
	handler_t *handler;
	char       buf[4096];

        struct timeval tv;

	record.name      = logger->name;
	record.level     = level;
	record.levelname = logger->level_table[level];

	vsnprintf(buf, sizeof(buf), msg, args);
	record.message = buf;

        gettimeofday(&tv, NULL);
	record.seconds  = tv.tv_sec;
	record.useconds = tv.tv_usec;

	for (filter = logger->filter; filter != NULL; filter = filter->next) {
		if (filter_filter(filter, &record) == 0)
			return;
	}

	for (handler = logger->handler; handler != NULL; handler = handler->next) {
		handler_emit(handler, &record);
	}
}

void
logger_debug(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, DEBUG, msg, args);

	va_end(args);
}

void
logger_info(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, INFO, msg, args);

	va_end(args);
}

void
logger_warning(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, WARNING, msg, args);

	va_end(args);
}

void
logger_error(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, ERROR, msg, args);

	va_end(args);
}

void
logger_critical(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, CRITICAL, msg, args);

	va_end(args);
}

void
logger_log(logger_t *logger, int level, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, level, msg, args);

	va_end(args);
}
