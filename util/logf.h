#ifndef CANON_C_UTIL_LOGF_H
#define CANON_C_UTIL_LOGF_H

#include <stdarg.h>
#include <stdio.h>
#include "log.h"

/*
    logf.h — formatted logging helpers

    Derived utility built on top of log.h.

    - No allocation
    - No global state
    - Header-only
    - Formatting only; output semantics remain in log.h
*/

/* ------------------------------------------------------------
   Formatted logging
   ------------------------------------------------------------ */

static inline void logf(LogLevel level, const char *fmt, ...) {
    const char *prefix = "";

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

#endif /* CANON_C_UTIL_LOGF_H */

