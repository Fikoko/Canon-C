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

    Writes pointers to substrings into `out`.
    Each substring is null-terminated in-place.

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

    size_t count = 0;
    char *p = s;

    out[count++] = p;

    while (*p && count < max_parts) {
        if (*p == delim) {
            *p = '\0';
            out[count++] = p + 1;
        }
        p++;
    }

    return count;
}

#endif /* CANON_C_UTIL_STR_SPLIT_H */

