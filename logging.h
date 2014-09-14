#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <stdio.h>
#include <sys/time.h>


#define LOGGING_MAX_LEVEL	60

enum {
	LOG_CRITICAL = 50,
	LOG_ERROR    = 40,
	LOG_WARNING  = 30,
	LOG_INFO     = 20,
	LOG_DEBUG    = 10,
	LOG_NOTSET   = 0
};

typedef struct logger_record {
	char *name;
	int   level;
	char *levelname;
	char *message;

	time_t      seconds;
	suseconds_t useconds;
} logger_record_t;

typedef struct logger_formatter {
	char *fmt;
	char *datefmt;
} logger_formatter_t;

typedef struct logger_filter {
	char *name;
	int   minlevel;	/* include */
	int   maxlevel; /* include */

	struct logger_filter *next;
} logger_filter_t;

typedef struct logger_handler {
	char        *name;
	FILE        *file;
	logger_filter_t    *filter;
	logger_formatter_t *formatter;

	struct logger_handler *next;
} logger_handler_t;

typedef struct logger {
	char *name;
	char *level_table[LOGGING_MAX_LEVEL];

	logger_filter_t  *filter;
	logger_handler_t *handler;

	struct logger *next;
} logger_t;

logger_t *
logging_getlogger(char name[]);

void
logging_setlogger(logger_t *logger);

char *
logger_getlevelname(logger_t *logger, int level);

void
logger_setlevelname(logger_t *logger, int level, char *name);

void
logger_debug(logger_t *logger, char *msg, ...);

void
logger_info(logger_t *logger, char *msg, ...);

void
logger_warning(logger_t *logger, char *msg, ...);

void
logger_error(logger_t *logger, char *msg, ...);

void
logger_critical(logger_t *logger, char *msg, ...);

void
logger_log(logger_t *logger, int level, char *msg, ...);


#endif /* __LOGGING_H__ */
