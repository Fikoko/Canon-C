#ifndef CANON_C_UTIL_LOG_H
#define CANON_C_UTIL_LOG_H

#include <stdio.h>
#include <stdarg.h>

/*
    log.h — minimal logging utility

    - No allocation
    - No global state
    - Header-only
*/

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

// Log simple message
static inline void log_msg(LogLevel level, const char* msg) {
    const char* prefix = "";

    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    printf("%s%s\n", prefix, msg);
}

// Log formatted message
static inline void log_fmt(LogLevel level, const char* fmt, ...) {
    const char* prefix = "";

    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    printf("%s", prefix);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

#endif /* CANON_C_UTIL_LOG_H */
