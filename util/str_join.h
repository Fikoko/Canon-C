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
   Manual join
   ------------------------------------------------------------ */

/*
    Joins `count` strings from `parts` using separator `sep`
    into `dest`.

    Semantics:
    - sep == NULL is treated as empty string
    - count == 0 produces empty string

    Returns:
    - true on success
    - false if dest_size is insufficient or input invalid

    Requirements:
    - dest must be writable
    - parts[i] must be non-NULL, null-terminated strings
*/
static inline bool str_join_manual(
    char       *dest,
    size_t      dest_size,
    const char **parts,
    size_t      count,
    const char *sep
) {
    if (!dest || dest_size == 0)
        return false;

    if (count == 0) {
        dest[0] = '\0';
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
    return true;
}

/* ------------------------------------------------------------
   Convenience wrapper: join after splitting
   ------------------------------------------------------------ */

/*
    Joins string `s` split by delimiter `delim` into `dest`
    using `sep` as output separator.

    Semantics:
    - Modifies `s` in-place for splitting
    - Uses `out_parts` buffer to hold split pointers
    - Returns true on success
*/
static inline bool str_join_split(
    char       *s,
    char        delim,
    char      **out_parts,
    size_t      max_parts,
    char       *dest,
    size_t      dest_size,
    const char *sep
) {
    if (!s || !out_parts || max_parts == 0 || !dest || dest_size == 0)
        return false;

    size_t count = 0;
    count = 0;

    /* Split the string in-place */
    char *p = s;
    while (*p == delim) p++; /* skip leading delimiters */

    if (*p != '\0')
        out_parts[count++] = p;

    while (*p && count < max_parts) {
        if (*p == delim) {
            *p = '\0';
            do { p++; } while (*p == delim);
            if (*p != '\0' && count < max_parts)
                out_parts[count++] = p;
            continue;
        }
        p++;
    }

    return str_join_manual(dest, dest_size, (const char **)out_parts, count, sep);
}

#endif /* CANON_C_UTIL_STR_JOIN_H */
