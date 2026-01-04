#ifndef CANON_C_SEMANTICS_ERROR_H
#define CANON_C_SEMANTICS_ERROR_H

#include "result.h"

/*
    error.h — Common error codes and messages

    Use with Result<T, Error>:
      Result<int, Error> parse_int(const char* s);
*/

typedef enum {
    ERR_OK = 0,              // not an error
    ERR_INVALID_ARG,
    ERR_OUT_OF_MEMORY,
    ERR_BUFFER_TOO_SMALL,
    ERR_NOT_FOUND,
    ERR_PARSE_FAILED,
    ERR_IO_FAILED,
    ERR_OVERFLOW,
    ERR_UNDERFLOW,
    ERR_PERMISSION,
    ERR_TIMEOUT,
    // Add more as needed...
    ERR_COUNT
} Error;

static inline const char* error_message(Error e)
{
    switch (e) {
        case ERR_OK:               return "No error";
        case ERR_INVALID_ARG:      return "Invalid argument";
        case ERR_OUT_OF_MEMORY:    return "Out of memory";
        case ERR_BUFFER_TOO_SMALL: return "Buffer too small";
        case ERR_NOT_FOUND:        return "Not found";
        case ERR_PARSE_FAILED:     return "Parse failed";
        case ERR_IO_FAILED:        return "I/O operation failed";
        case ERR_OVERFLOW:         return "Numeric overflow";
        case ERR_UNDERFLOW:        return "Numeric underflow";
        case ERR_PERMISSION:       return "Permission denied";
        case ERR_TIMEOUT:          return "Operation timed out";
        default:                   return "Unknown error";
    }
}

#define RESULT_OK(T, val)          result_##T##_Error_ok(val)
#define RESULT_ERR(T, err_code)    result_##T##_Error_err(err_code)

#endif /* CANON_C_SEMANTICS_ERROR_H */
