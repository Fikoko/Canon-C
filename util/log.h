#ifndef CANON_C_UTIL_LOG_H
#define CANON_C_UTIL_LOG_H

#include <stdio.h>
#include <stdarg.h>

/*
    log.h — minimal observable logging

    This module:
    - Performs explicit side effects (I/O)
    - Does not allocate
    - Does not store global state
    - Does not impose logging policy

    Output destination is always explicit.
*/

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

/* ============================================================
   Core logging
   ============================================================ */

/* Log simple message to explicit stream */
static inline void log_to(
    FILE     *out,
    LogLevel  level,
    const char *msg
) {
    if (!out || !msg) return;

    const char *prefix = "";

    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    fprintf(out, "%s%s\n", prefix, msg);
}

/* Log formatted message to explicit stream */
static inline void log_fmt_to(
    FILE     *out,
    LogLevel  level,
    const char *fmt,
    ...
) {
    if (!out || !fmt) return;

    const char *prefix = "";

    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    fprintf(out, "%s", prefix);

    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);

    fputc('\n', out);
}

/* ============================================================
   Convenience helpers
   ============================================================ */

static inline void log_msg(LogLevel level, const char *msg) {
    FILE *out = (level == LOG_ERROR) ? stderr : stdout;
    log_to(out, level, msg);
}

static inline void log_fmt(LogLevel level, const char *fmt, ...) {
    FILE *out = (level == LOG_ERROR) ? stderr : stdout;

    va_list args;
    va_start(args, fmt);

    const char *prefix = "";
    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    fprintf(out, "%s", prefix);
    vfprintf(out, fmt, args);
    fputc('\n', out);

    va_end(args);
}

#endif /* CANON_C_UTIL_LOG_H */
