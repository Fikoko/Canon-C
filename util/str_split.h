#ifndef CANON_C_UTIL_STR_SPLIT_H
#define CANON_C_UTIL_STR_SPLIT_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
    str_split.h — split string into substrings

    Derived utility.
    - No allocation
    - No ownership transfer
    - Output substrings reference the original string
    - Caller controls storage of output pointers
*/

/* ------------------------------------------------------------
   Core split
   ------------------------------------------------------------ */

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

/* ------------------------------------------------------------
   Optional helpers
   ------------------------------------------------------------ */

/*
    Rejoins split substrings into caller buffer using separator.
    - parts   : array of substrings (read-only)
    - count   : number of substrings
    - dest    : output buffer (caller-owned)
    - dest_sz : buffer size
    - sep     : separator string (NULL treated as "")
    Returns true on success, false if buffer too small or invalid input
*/
static inline bool str_split_join(
    const char **parts,
    size_t       count,
    char        *dest,
    size_t       dest_sz,
    const char  *sep
) {
    if (!parts || !dest || dest_sz == 0)
        return false;

    if (!sep)
        sep = "";

    size_t sep_len = strlen(sep);
    size_t pos = 0;

    for (size_t i = 0; i < count; i++) {
        if (!parts[i])
            return false;

        size_t part_len = strlen(parts[i]);
        if (pos + part_len + 1 > dest_sz)
            return false;

        memcpy(dest + pos, parts[i], part_len);
        pos += part_len;

        if (i + 1 < count && sep_len > 0) {
            if (pos + sep_len + 1 > dest_sz)
                return false;

            memcpy(dest + pos, sep, sep_len);
            pos += sep_len;
        }
    }

    dest[pos] = '\0';
    return true;
}

/*
    Trim leading and trailing characters from a substring in-place.
    - s        : mutable substring
    - trim_ch  : character to trim
    Returns pointer to trimmed string (may be same as input)
*/
static inline char *str_trim(char *s, char trim_ch) {
    if (!s)
        return NULL;

    /* Trim leading */
    while (*s == trim_ch)
        s++;

    /* Trim trailing */
    char *end = s + strlen(s);
    while (end > s && *(end - 1) == trim_ch)
        *(--end) = '\0';

    return s;
}

#endif /* CANON_C_UTIL_STR_SPLIT_H */
