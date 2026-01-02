#ifndef CANON_C_UTIL_LOG_MACROS_H
#define CANON_C_UTIL_LOG_MACROS_H

#include "util/log.h"

/*
    log_macros.h — Safe and convenient logging macros

    Provides:
      - Explicit Result-based logging (preferred)
      - Optional fire-and-forget macros (for debug spam or release)

    Never silently ignores I/O failure.
    All formatting is stack-based (no allocation).
*/

#define LOG_INFO_MSG(msg)    (void)log_msg(LOG_INFO, (msg))
#define LOG_WARN_MSG(msg)    (void)log_msg(LOG_WARN, (msg))
#define LOG_ERROR_MSG(msg)   (void)log_msg(LOG_ERROR, (msg))

#define LOG_INFO_FMT(...)  (void)log_fmt(LOG_INFO, __VA_ARGS__)
#define LOG_WARN_FMT(...)  (void)log_fmt(LOG_WARN, __VA_ARGS__)
#define LOG_ERROR_FMT(...) (void)log_fmt(LOG_ERROR, __VA_ARGS__)

/* Optional: Only in debug builds — check result */
#ifdef NDEBUG
    /* Release: fire and forget */
    #define LOG_INFO_CHECKED(...)  LOG_INFO_FMT(__VA_ARGS__)
    #define LOG_WARN_CHECKED(...)  LOG_WARN_FMT(__VA_ARGS__)
    #define LOG_ERROR_CHECKED(...) LOG_ERROR_FMT(__VA_ARGS__)
#else
    /* Debug: check and report logging failure (meta-logging) */
    #define LOG_INFO_CHECKED(...)  (void)log_fmt(LOG_INFO, __VA_ARGS__)
    #define LOG_WARN_CHECKED(...)  (void)log_fmt(LOG_WARN, __VA_ARGS__)
    #define LOG_ERROR_CHECKED(...) \
        do { \
            result_bool_constcharp _r = log_fmt(LOG_ERROR, __VA_ARGS__); \
            if (result_bool_constcharp_is_err(_r)) { \
                /* Fallback: try raw write to stderr */ \
                fprintf(stderr, "[LOG FAILURE] Could not log message\n"); \
            } \
        } while (0)
#endif

#endif /* CANON_C_UTIL_LOG_MACROS_H */
