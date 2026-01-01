#ifndef CANON_C_UTIL_STR_SPLIT_H
#define CANON_C_UTIL_STR_SPLIT_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

/*
    str_split.h — split string into substrings

    Derived utility.

    Semantics:
    - No allocation
    - No ownership transfer
    - Output substrings reference the original string
    - Caller controls storage of output pointers
    - Modifies input string in-place (inserts '\0' terminators)
*/

/*
    Splits string `s` by delimiter `delim`.

    Parameters:
    - s        : mutable null-terminated string to split
    - delim    : delimiter character; '\0' means no split
    - out      : output array of char* pointers
    - max_parts: maximum number of output substrings

    Returns:
    - number of substrings written to `out` (<= max_parts)

    Notes:
    - Leading delimiters are skipped
    - Consecutive delimiters are treated as single separators
    - Remaining parts exceeding max_parts are ignored
    - Caller must ensure `out` has capacity >= max_parts
*/
static inline size_t str_split(
    char   *s,
    char    delim,
    char  **out,
    size_t  max_parts
) {
    assert(out != NULL && "Output buffer must not be NULL");

    if (!s || !out || max_parts == 0)
        return 0;

    /* Special case: no delimiter → whole string is one part */
    if (delim == '\0') {
        out[0] = s;
        return 1;
    }

    size_t count = 0;
    char *p = s;

    /* Skip leading delimiters */
    while (*p == delim)
        p++;

    if (*p == '\0')
        return 0;

    out[count++] = p;

    while (*p && count < max_parts) {
        if (*p == delim) {
            *p = '\0';

            /* Skip consecutive delimiters */
            do {
                p++;
            } while (*p == delim);

            if (*p == '\0')
                break;

            if (count < max_parts)
                out[count++] = p;
            continue;
        }
        p++;
    }

    return count;
}

#endif /* CANON_C_UTIL_STR_SPLIT_H */
