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
   Manual mode with optional output
   ------------------------------------------------------------ */

/*
    Joins `count` strings from `parts` using separator `sep`
    into `dest`.

    Semantics:
    - sep == NULL is treated as empty string
    - count == 0 produces empty string
    - dest_written optionally receives the number of characters written (excluding null terminator)

    Returns:
    - true  on success
    - false if dest_size is insufficient or input invalid

    Requirements:
    - dest must be writable
    - parts[i] must be non-NULL, null-terminated strings
    - dest_size must be >= sum(strlen(parts[i])) + strlen(sep)*(count-1) + 1
*/
static inline bool str_join_manual_ex(
    char       *dest,
    size_t      dest_size,
    const char **parts,
    size_t      count,
    const char *sep,
    size_t     *dest_written
) {
    if (!dest || dest_size == 0)
        return false;

    if (count == 0) {
        dest[0] = '\0';
        if (dest_written) *dest_written = 0;
        return true;
    }

    if (!parts)
        return false;

    if (!sep)
        sep = "";

    size_t pos = 0;
    size_t sep_len = strlen(sep);

    for (size_t i = 0; i < count; i++) {
        if (!parts[i])
            return false;

        size_t part_len = strlen(parts[i]);

        if (pos + part_len + 1 > dest_size)
            return false;

        memcpy(dest + pos, parts[i], part_len);
        pos += part_len;

        if (i + 1 < count && sep_len > 0) {
            if (pos + sep_len + 1 > dest_size)
                return false;

            memcpy(dest + pos, sep, sep_len);
            pos += sep_len;
        }
    }

    dest[pos] = '\0';
    if (dest_written)
        *dest_written = pos;

    return true;
}

/* ------------------------------------------------------------
   Convenience macro for Vec_<char*> or array
   ------------------------------------------------------------ */

/*
    STR_JOIN_VEC(dest, dest_size, vec_or_array, sep)

    - dest           : destination buffer
    - dest_size      : size of destination buffer
    - vec_or_array   : array of char* or Vec_<char*>
    - sep            : separator string (may be NULL)

    Returns:
    - true on success
*/
#define STR_JOIN_VEC(dest, dest_size, vec_or_array, sep) \
    str_join_manual_ex((dest), (dest_size), \
        (vec_or_array).items, (vec_or_array).len, (sep), NULL)

#endif /* CANON_C_UTIL_STR_JOIN_H */
