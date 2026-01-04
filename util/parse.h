#ifndef CANON_C_UTIL_PARSE_H
#define CANON_C_UTIL_PARSE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "semantics/result.h"
#include "semantics/error.h"

/*
    parse.h — Safe parsing of common types from strings

    All functions:
      - Consume input prefix
      - Return Result with parsed value and advanced pointer
      - Never mutate input
*/

CANON_C_DEFINE_RESULT(int64_t, Error)
CANON_C_DEFINE_RESULT(uint64_t, Error)
CANON_C_DEFINE_RESULT(double, Error)

/* Parse signed integer (decimal, hex 0x, octal 0) */
static inline result_int64_t_Error parse_int64(const char* s, const char** endptr)
{
    if (!s || !*s) return RESULT_ERR(int64_t, ERR_PARSE_FAILED);

    char* eptr;
    int64_t val = strtoll(s, &eptr, 0);
    if (eptr == s) return RESULT_ERR(int64_t, ERR_PARSE_FAILED);

    if (endptr) *endptr = eptr;
    return RESULT_OK(int64_t, val);
}

/* Parse unsigned integer */
static inline result_uint64_t_Error parse_uint64(const char* s, const char** endptr)
{
    if (!s || !*s) return RESULT_ERR(uint64_t, ERR_PARSE_FAILED);

    char* eptr;
    uint64_t val = strtoull(s, &eptr, 0);
    if (eptr == s) return RESULT_ERR(uint64_t, ERR_PARSE_FAILED);

    if (endptr) *endptr = eptr;
    return RESULT_OK(uint64_t, val);
}

/* Parse double */
static inline result_double_Error parse_double(const char* s, const char** endptr)
{
    if (!s || !*s) return RESULT_ERR(double, ERR_PARSE_FAILED);

    char* eptr;
    double val = strtod(s, &eptr);
    if (eptr == s) return RESULT_ERR(double, ERR_PARSE_FAILED);

    if (endptr) *endptr = eptr;
    return RESULT_OK(double, val);
}

/* Skip whitespace */
static inline const char* parse_skip_ws(const char* s)
{
    while (*s && isspace((unsigned char)*s)) ++s;
    return s;
}

#endif /* CANON_C_UTIL_PARSE_H */
