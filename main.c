#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
hello()
{
	logger_t *logger;

	logger = logging_getlogger("foo");

	logger_log(logger, 21, "%s", "log from hello");
}

int
main(void)
{
	FILE *file1;
	FILE *file2;

	logger_t  logger;

	logger_filter_t  filter1;
	logger_filter_t  filter2;

	logger_handler_t handler1;
	logger_handler_t handler2;

	logger_formatter_t formatter1;
	logger_formatter_t formatter2;

	memset(&logger, 0, sizeof(logger));
	logger.name = "foo";
	logger_setlevelname(&logger, 21, "HELLO");

	filter1.minlevel = 0;
	filter1.maxlevel = LOGGING_MAX_LEVEL;
	filter1.next     = NULL;

	filter2.minlevel = 20;
	filter2.maxlevel = LOGGING_MAX_LEVEL;
	filter2.next     = NULL;

	formatter1.datefmt = "%Y-%m-%dT%H:%M:%SZ";
	formatter1.fmt = "%(asctime)s	"
			 "%(usecs)d	"
			 "%(levelno)d	"
			 "%(levelname)s	"
			 "%(message)s";

	formatter2.datefmt = "%Y-%m-%dT%H:%M:%SZ";
	formatter2.fmt = "%(asctime)s,"
			 "%(usecs)d,"
			 "%(levelno)d,"
			 "%(levelname)s,"
			 "%(message)s";

	file1 = fopen("error.log", "a");

	handler1.name      = "HANDLER1";
	handler1.file      = file1;
	handler1.filter    = &filter1;
	handler1.formatter = &formatter1;
	handler1.next      = &handler2;

	file2 = fopen("access.log", "a");

	handler2.name      = "HANDLER2";
	handler2.file      = file2;
	handler2.filter    = &filter2;
	handler2.formatter = &formatter2;
	handler2.next      = NULL;


	logger.filter  = NULL;
	logger.handler = &handler1;

	logging_setlogger(&logger);

	logger_debug(&logger, "%s", "hello,world");
	logger_info(&logger, "%s", "hello,world");
	logger_log(&logger, 21, "%s", "hello,world");

	hello();
}

