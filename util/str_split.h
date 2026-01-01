#ifndef CANON_C_UTIL_STR_SPLIT_H
#define CANON_C_UTIL_STR_SPLIT_H

#include <stddef.h>
#include <stdbool.h>

/*
    str_split.h — split string into substrings

    Derived utility.
    - No allocation
    - No ownership transfer
    - Output substrings reference the original string
    - Caller controls storage of output pointers
*/

/*
    Splits string `s` by delimiter `delim`.

    Semantics:
    - Modifies `s` in-place by inserting '\0'
    - Skips leading delimiters
    - Stops writing when max_parts is reached
    - Remaining string (if any) is left intact

    Special case:
    - delim == '\0' → no split, returns single part

    Returns:
    - number of substrings written

    Requirements:
    - `s` must be mutable
    - `out` must have capacity >= max_parts
*/
static inline size_t str_split(
    char   *s,
    char    delim,
    char  **out,
    size_t  max_parts
) {
    if (!s || !out || max_parts == 0)
        return 0;

    /* No delimiter: whole string is one part */
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
