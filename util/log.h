#ifndef CANON_C_UTIL_LOG_H
#define CANON_C_UTIL_LOG_H

#include <stdio.h>

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

// Basic logging function
static inline void log_msg(LogLevel level, const char* msg) {
    const char* prefix = "";
    switch(level) {
        case LOG_INFO:  prefix = "[INFO] "; break;
        case LOG_WARN:  prefix = "[WARN] "; break;
        case LOG_ERROR: prefix = "[ERROR] "; break;
    }
    printf("%s%s\n", prefix, msg);
}

#endif // CANON_C_UTIL_LOG_H
