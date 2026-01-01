#ifndef CANON_C_UTIL_LOG_H
#define CANON_C_UTIL_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <canon/semantics/result.h>

/*
    log.h — minimal observable logging (Canon-C Result-based)

    This module:
    - Performs explicit side effects (I/O)
    - Does not allocate
    - Does not store global state
    - Returns explicit Result on failure
*/

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

/* Result type for logging functions */
CANON_C_DEFINE_RESULT(void, const char*)  /* Result<void, const char*> */

/* ============================================================
   Internal primitive
   ============================================================ */

/* Log formatted message with va_list to explicit stream */
static inline Result_void_const_char_ptr log_vfmt_to(
    FILE      *out,
    LogLevel   level,
    const char *fmt,
    va_list    args
) {
    if (!out) return Result_void_const_char_ptr_Err("Output stream NULL");
    if (!fmt) return Result_void_const_char_ptr_Err("Format string NULL");

    const char *prefix = "";

    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    if (fputs(prefix, out) == EOF) 
        return Result_void_const_char_ptr_Err("Failed to write prefix");

    if (vfprintf(out, fmt, args) < 0)
        return Result_void_const_char_ptr_Err("Failed to write formatted message");

    if (fputc('\n', out) == EOF)
        return Result_void_const_char_ptr_Err("Failed to write newline");

    return Result_void_const_char_ptr_Ok(NULL);
}

/* ============================================================
   Core logging
   ============================================================ */

/* Log simple message to explicit stream */
static inline Result_void_const_char_ptr log_to(
    FILE      *out,
    LogLevel   level,
    const char *msg
) {
    if (!out) return Result_void_const_char_ptr_Err("Output stream NULL");
    if (!msg) return Result_void_const_char_ptr_Err("Message NULL");

    const char *prefix = "";

    switch (level) {
        case LOG_INFO:  prefix = "[INFO] ";  break;
        case LOG_WARN:  prefix = "[WARN] ";  break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }

    if (fprintf(out, "%s%s\n", prefix, msg) < 0)
        return Result_void_const_char_ptr_Err("Failed to write message");

    return Result_void_const_char_ptr_Ok(NULL);
}

/* Log formatted message to explicit stream */
static inline Result_void_const_char_ptr log_fmt_to(
    FILE      *out,
    LogLevel   level,
    const char *fmt,
    ...
) {
    if (!out) return Result_void_const_char_ptr_Err("Output stream NULL");
    if (!fmt) return Result_void_const_char_ptr_Err("Format string NULL");

    va_list args;
    va_start(args, fmt);
    Result_void_const_char_ptr res = log_vfmt_to(out, level, fmt, args);
    va_end(args);
    return res;
}

/* ============================================================
   Convenience helpers
   ============================================================ */

/* Log simple message to default stream (stdout / stderr) */
static inline Result_void_const_char_ptr log_msg(LogLevel level, const char *msg) {
    if (!msg) return Result_void_const_char_ptr_Err("Message NULL");

    FILE *out = (level == LOG_ERROR) ? stderr : stdout;
    return log_to(out, level, msg);
}

/* Log formatted message to default stream (stdout / stderr) */
static inline Result_void_const_char_ptr log_fmt(LogLevel level, const char *fmt, ...) {
    if (!fmt) return Result_void_const_char_ptr_Err("Format string NULL");

    FILE *out = (level == LOG_ERROR) ? stderr : stdout;

    va_list args;
    va_start(args, fmt);
    Result_void_const_char_ptr res = log_vfmt_to(out, level, fmt, args);
    va_end(args);
    return res;
}

#endif /* CANON_C_UTIL_LOG_H */
