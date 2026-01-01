#ifndef CANON_C_UTIL_LOG_H
#define CANON_C_UTIL_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

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
   Internal primitive
   ============================================================ */

/* Log formatted message with va_list to explicit stream */
static inline void log_vfmt_to(
    FILE      *out,
    LogLevel   level,
    const char *fmt,
    va_list    args
) {
    if (!out || !fmt) return;

    const char *prefix = "";

    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    fputs(prefix, out);
    vfprintf(out, fmt, args);
    fputc('\n', out);
}

/* ============================================================
   Core logging
   ============================================================ */

/* Log simple message to explicit stream */
static inline void log_to(
    FILE      *out,
    LogLevel   level,
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
    FILE      *out,
    LogLevel   level,
    const char *fmt,
    ...
) {
    if (!out || !fmt) return;

    va_list args;
    va_start(args, fmt);
    log_vfmt_to(out, level, fmt, args);
    va_end(args);
}

/* ============================================================
   Convenience helpers
   ============================================================ */

/* Log simple message to default stream (stdout / stderr) */
static inline void log_msg(LogLevel level, const char *msg) {
    if (!msg) return;

    FILE *out = (level == LOG_ERROR) ? stderr : stdout;
    log_to(out, level, msg);
}

/* Log formatted message to default stream (stdout / stderr) */
static inline void log_fmt(LogLevel level, const char *fmt, ...) {
    if (!fmt) return;

    FILE *out = (level == LOG_ERROR) ? stderr : stdout;

    va_list args;
    va_start(args, fmt);
    log_vfmt_to(out, level, fmt, args);
    va_end(args);
}

#endif /* CANON_C_UTIL_LOG_H */
