
Simple Logging Library for C
Learning from Python logging module

Demo See main.c

This Library is Cutting Edge, Please DON'T use unless you know what're you doing.

Public Domain License

Update:
This project is no longer maintained,use syslog instead.

syslog helper:

```
#include <syslog.h>

#define log_debug(fmt, ...)   syslog(LOG_DEBUG,   fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    syslog(LOG_INFO,    fmt, ##__VA_ARGS__)
#define log_notice(fmt, ...)  syslog(LOG_NOTICE,  fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) syslog(LOG_WARNING, fmt, ##__VA_ARGS__)
#define log_err(fmt, ...)     syslog(LOG_ERR,     fmt, ##__VA_ARGS__)
#define log_crit(fmt, ...)    syslog(LOG_CRIT,    fmt, ##__VA_ARGS__)
#define log_alert(fmt, ...)   syslog(LOG_ALERT,   fmt, ##__VA_ARGS__)
#define log_emerg(fmt, ...)   syslog(LOG_EMERG,   fmt, ##__VA_ARGS__)
```
