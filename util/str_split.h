#ifndef CANON_C_UTIL_STR_SPLIT_H
#define CANON_C_UTIL_STR_SPLIT_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
    str_split.h — Safe, non-mutating string splitting utilities

    All functions:
      - Never mutate input strings (const-correct)
      - Never allocate
      - Never take ownership
      - Output parts reference original string (borrowed views)
      - Caller provides storage for pointers

    Perfect for parsing without copying or destroying input.
*/

#include "util/str_join.h"  // for str_join, str_alloc_join (optional include)

/* ============================================================
   Core: Non-mutating split (safe, recommended)
   ============================================================ */

/*
    str_split:
      Splits const string `s` by `delim` into `out_parts` buffer.
      - Does NOT modify `s`
      - Skips empty segments (consecutive delimiters → one split)
      - Stops at max_parts
      - Returns number of parts written
      - Substrings are borrowed views into `s` (valid as long as `s` lives)
*/
static inline size_t str_split(
    const char* s,
    char delim,
    const char** out_parts,
    size_t max_parts
)
{
    if (!s || !out_parts || max_parts == 0) return 0;

    /* Special case: no delimiter → single part */
    if (delim == '\0') {
        out_parts[0] = s;
        return 1;
    }

    size_t count = 0;
    const char* start = s;

    while (*start) {
        /* Skip leading delimiters */
        while (*start == delim) ++start;
        if (*start == '\0') break;

        if (count < max_parts) {
            out_parts[count++] = start;
        }

        /* Find end of part */
        while (*start && *start != delim) ++start;

        /* If at delimiter, next loop will skip it */
    }

    return count;
}

/* ============================================================
   Trimming (in-place, explicit mutation)
   ============================================================ */

/*
    str_trim_in_place:
      Removes leading and trailing instances of `trim_ch` from `s`.
      - Modifies `s` in-place (explicitly named)
      - Returns pointer to start of trimmed content (may shift)
*/
static inline char* str_trim_in_place(char* s, char trim_ch)
{
    if (!s) return NULL;

    /* Trim leading */
    char* start = s;
    while (*start == trim_ch) ++start;

    /* If entire string was trim chars */
    if (*start == '\0') {
        s[0] = '\0';
        return s;
    }

    /* Move content if needed */
    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }

    /* Trim trailing */
    char* end = s + strlen(s);
    while (end > s && *(end - 1) == trim_ch) --end;
    *end = '\0';

    return s;
}

/*
    str_trim_whitespace:
      Convenience: trim common whitespace (\t\n\r )
*/
static inline char* str_trim_whitespace(char* s)
{
    if (!s) return NULL;

    char* start = s;
    while (*start && strchr(" \t\n\r", *start)) ++start;

    if (*start == '\0') {
        s[0] = '\0';
        return s;
    }

    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }

    char* end = s + strlen(s);
    while (end > s && strchr(" \t\n\r", *(end - 1))) --end;
    *end = '\0';

    return s;
}

#endif /* CANON_C_UTIL_STR_SPLIT_H */
