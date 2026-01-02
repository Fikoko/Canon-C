#ifndef CANON_C_UTIL_LOG_H
#define CANON_C_UTIL_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include "semantics/result.h"

/*
    log.h — Minimal, explicit, observable logging

    Features:
      - Explicit output stream control
      - No allocation, no global state
      - Explicit Result<void, const char*> on failure
      - Side effects are intentional and visible
      - Errors go to stderr, info/warn to stdout

    This is an optional utility — safe to include everywhere.
*/

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level;

/* Result type: success = Ok(()), failure = Err(message) */
CANON_C_DEFINE_RESULT(bool, const char*)  // true = success

/* ============================================================
   Core: Log to explicit stream
   ============================================================ */

/* Low-level: formatted logging with va_list */
static inline result_bool_constcharp log_vfmt_to(
    FILE* stream,
    log_level level,
    const char* fmt,
    va_list args
)
{
    if (!stream) return result_bool_constcharp_err("null output stream");
    if (!fmt)    return result_bool_bool_constcharp_err("null format string");

    const char* prefix = "";
    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    if (fputs(prefix, stream) == EOF)
        return result_bool_constcharp_err("failed to write log prefix");

    if (vfprintf(stream, fmt, args) < 0)
        return result_bool_constcharp_err("failed to write formatted message");

    if (fputc('\n', stream) == EOF)
        return result_bool_constcharp_err("failed to write newline");

    if (fflush(stream) == EOF)
        return result_bool_constcharp_err("failed to flush stream");

    return result_bool_constcharp_ok(true);
}

/* Formatted logging to explicit stream */
static inline result_bool_constcharp log_fmt_to(
    FILE* stream,
    log_level level,
    const char* fmt,
    ...
)
{
    va_list args;
    va_start(args, fmt);
    result_bool_constcharp res = log_vfmt_to(stream, level, fmt, args);
    va_end(args);
    return res;
}

/* Simple string message to explicit stream */
static inline result_bool_constcharp log_to(
    FILE* stream,
    log_level level,
    const char* msg
)
{
    if (!msg) return result_bool_constcharp_err("null message");
    return log_fmt_to(stream, level, "%s", msg);
}

/* ============================================================
   Convenience: Default streams (stdout / stderr)
   ============================================================ */

/* Formatted logging to default stream */
static inline result_bool_constcharp log_fmt(
    log_level level,
    const char* fmt,
    ...
)
{
    FILE* stream = (level == LOG_ERROR) ? stderr : stdout;

    va_list args;
    va_start(args, fmt);
    result_bool_constcharp res = log_vfmt_to(stream, level, fmt, args);
    va_end(args);
    return res;
}

/* Simple message to default stream */
static inline result_bool_constcharp log_msg(
    log_level level,
    const char* msg
)
{
    FILE* stream = (level == LOG_ERROR) ? stderr : stdout;
    return log_to(stream, level, msg);
}

/* ============================================================
   Macros for fire-and-forget (common in release builds)
   ============================================================ */

#define LOG_INFO(msg)    (void)log_msg(LOG_INFO, (msg))
#define LOG_WARN(msg)    (void)log_msg(LOG_WARN, (msg))
#define LOG_ERROR(msg)   (void)log_msg(LOG_ERROR, (msg))

#define LOG_INFO_FMT(...)  (void)log_fmt(LOG_INFO, __VA_ARGS__)
#define LOG_WARN_FMT(...)  (void)log_fmt(LOG_WARN, __VA_ARGS__)
#define LOG_ERROR_FMT(...) (void)log_fmt(LOG_ERROR, __VA_ARGS__)

#endif /* CANON_C_UTIL_LOG_H */
