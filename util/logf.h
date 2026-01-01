#ifndef CANON_C_UTIL_LOGF_H
#define CANON_C_UTIL_LOGF_H

#include <stdarg.h>
#include "log.h"

/*
    logf.h — formatted logging helpers

    Derived utility built on top of log.h.

    Semantics:
    - Formatting only
    - Output behavior is delegated to log.h
    - No allocation
    - No global state
    - Header-only
*/

/*
    Formatted logging wrapper.
    Delegates output semantics to log.h.
*/
static inline void logf(LogLevel level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_vfmt(level, fmt, args);
    va_end(args);
}

#endif /* CANON_C_UTIL_LOGF_H */
