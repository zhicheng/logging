#include "logging.h"

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
	logger_t  logger;

	filter_t  filter1;
	filter_t  filter2;

	handler_t handler1;
	handler_t handler2;

	formatter_t formatter1;
	formatter_t formatter2;

	logger.name = "foo";

	logger_setlevelname(&logger, 21, "HELLO");

	filter1.minlevel = 0;
	filter1.maxlevel = LOGGING_MAX_LEVEL;
	filter1.next     = NULL;

	filter2.minlevel = 20;
	filter2.maxlevel = LOGGING_MAX_LEVEL;
	filter2.next     = NULL;

	formatter1.datefmt = "%Y-%m-%dT%H:%M:%S.000Z";
	formatter1.fmt = "%(levelno)s	"
			 "%(levelname)s	"
			 "%(message)s	"
			 "%(created)s	"
			 "%(asctime)s	"
			 "%(msecs)s	"
			 "%(usecs)s";

	formatter2.datefmt = "%Y-%m-%dT%H:%M:%S.000Z";
	formatter2.fmt = "%(levelno)s,"
			 "%(levelname)s,"
			 "%(message)s,"
			 "%(created)s,"
			 "%(asctime)s,"
			 "%(msecs)s,"
			 "%(usecs)s";

	handler1.name      = "HANDLER1";
	handler1.filter    = &filter1;
	handler1.formatter = &formatter1;
	handler1.next      = &handler2;

	handler2.name      = "HANDLER2";
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

