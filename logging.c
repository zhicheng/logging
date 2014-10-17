#include "logging.h"

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

static char *
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
	int i;

	logger_t **next;

	for (i = 0; i < sizeof(level_table)/sizeof(level_table[0]); i++) {
		if (logger->level_table[i] == NULL)
			logger->level_table[i] = level_table[i];
	}
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
filter_filter(logger_filter_t *filter, logger_record_t *record)
{
	if (record->level < filter->minlevel)
		return 0;
	if (record->level > filter->maxlevel)
		return 0;
	return 1;
}

static int
formatter_format(logger_formatter_t *formatter, logger_record_t *record,
	char buf[], int buflen)
{
	int i;
	int len;
	int skip;
	int fmtlen;

	char *fmt;

	fmt = formatter->fmt;
	fmtlen = strlen(formatter->fmt);

#define min(a,b)	((a) > (b) ? (b) : (a))

	len = 0;
	for (i = 0; i < fmtlen && len < buflen; i += skip) {
		if (memcmp(fmt + i, "%%", 2) == 0) {
			buf[len++] = '%';
			skip = 2;
		} else if (memcmp(fmt + i, "%(name)s", 8) == 0) {
			int n = strlen(record->name);
			n = min(n, buflen - len);
			memcpy(&buf[len], record->name, n);
			len += n;
			skip = 8;
		} else if (memcmp(fmt + i, "%(levelno)d", 11) == 0) {
			int n = snprintf(&buf[len], buflen - len, "%d", record->level);
			len += n;
			skip = 11;
		} else if (memcmp(fmt + i, "%(levelname)s", 13) == 0) {
			int n = strlen(record->levelname);
			n = min(n, buflen - len);
			memcpy(&buf[len], record->levelname, n);
			len += n;
			skip = 13;
		} else if (memcmp(fmt + i, "%(message)s", 11) == 0) {
			int n = strlen(record->message);
			n = min(n, buflen - len);
			memcpy(&buf[len], record->message, n);
			len += n;
			skip = 11;
		} else if (memcmp(fmt + i, "%(created)d", 11) == 0) {
			int n = snprintf(&buf[len], buflen - len, "%ld", record->seconds);
			len += n;
			skip = 11;
		} else if (memcmp(fmt + i, "%(asctime)s", 11) == 0) {
			int n;
			struct tm tm;
			gmtime_r(&record->seconds, &tm);
			n = strftime(&buf[len], buflen - len, formatter->datefmt, &tm);
			len += n;
			skip = 11;
		} else if (memcmp(fmt + i, "%(msecs)d", 9) == 0) {
			int n = snprintf(&buf[len], buflen - len, "%d", record->useconds/1000);
			len += n;
			skip = 9;
		} else if (memcmp(fmt + i, "%(usecs)d", 9) == 0) {
			int n = snprintf(&buf[len], buflen - len, "%d", record->useconds);
			len += n;
			skip = 9;
		} else {
			buf[len++] = fmt[i];
			skip = 1;
		}
	}

#undef min

	return len;
}

static void
handler_emit(logger_handler_t *handler, logger_record_t *record)
{
	int  len;
	char buf[4096];
	logger_filter_t *filter;

	for (filter = handler->filter; filter != NULL; filter = filter->next) {
		if (filter_filter(filter, record) == 0)
			return;
	}

	len = formatter_format(handler->formatter, record, buf, sizeof(buf) - 1);
	buf[len++] = '\n';
	fwrite(buf, sizeof(char), len, handler->file);
	fflush(handler->file);
}

static void
log(logger_t *logger, int level, char *msg, va_list args)
{
	logger_record_t   record;
	logger_filter_t  *filter;
	logger_handler_t *handler;
	char              buf[4096];

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

	log(logger, LOG_DEBUG, msg, args);

	va_end(args);
}

void
logger_info(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, LOG_INFO, msg, args);

	va_end(args);
}

void
logger_warning(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, LOG_WARNING, msg, args);

	va_end(args);
}

void
logger_error(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, LOG_ERROR, msg, args);

	va_end(args);
}

void
logger_critical(logger_t *logger, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	log(logger, LOG_CRITICAL, msg, args);

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
