#ifndef CANON_C_UTIL_STR_JOIN_H
#define CANON_C_UTIL_STR_JOIN_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
    str_join.h — join multiple strings

    Derived utility.
    - No hidden allocation
    - No ownership transfer
    - Caller controls destination buffer
*/

/* ------------------------------------------------------------
   Manual mode (preferred)
   ------------------------------------------------------------ */

/*
    Joins `count` strings from `parts` using separator `sep`
    into `dest`.

    Returns:
    - true on success
    - false if dest_size is insufficient

    Requirements:
    - dest must be writable
    - parts must point to null-terminated strings
*/
static inline bool str_join_manual(
    char       *dest,
    size_t      dest_size,
    const char **parts,
    size_t      count,
    const char *sep
) {
    if (!dest || !parts || !sep)
        return false;

    size_t pos = 0;
    size_t sep_len = strlen(sep);

    for (size_t i = 0; i < count; i++) {
        size_t part_len = strlen(parts[i]);

        if (pos + part_len + 1 > dest_size)
            return false;

        memcpy(dest + pos, parts[i], part_len);
        pos += part_len;

        if (i + 1 < count) {
            if (pos + sep_len + 1 > dest_size)
                return false;

            memcpy(dest + pos, sep, sep_len);
            pos += sep_len;
        }
    }

    dest[pos] = '\0';
    return true;
}

#endif /* CANON_C_UTIL_STR_JOIN_H */

