#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <time.h>
#include <sys/time.h>


#define LOGGING_MAX_LEVEL	60

enum {
	CRITICAL = 50,
	ERROR    = 40,
	WARNING  = 30,
	INFO     = 20,
	DEBUG    = 10,
	NOTSET   = 0
};

typedef struct record {
	char *name;
	int   level;
	char *levelname;
	char *message;

	time_t      seconds;
	suseconds_t useconds;
} record_t;

typedef struct formatter {
	char *fmt;
	char *datefmt;
} formatter_t;

typedef struct filter {
	char *name;
	int   minlevel;	/* include */
	int   maxlevel; /* include */

	struct filter *next;
} filter_t;

typedef struct handler {
	char        *name;
	filter_t    *filter;
	formatter_t *formatter;

	struct handler *next;
} handler_t;

typedef struct logger {
	char *name;
	char *level_table[LOGGING_MAX_LEVEL];

	filter_t  *filter;
	handler_t *handler;

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
