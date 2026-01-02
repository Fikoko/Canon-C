#ifndef CANON_C_UTIL_STR_JOIN_H
#define CANON_C_UTIL_STR_JOIN_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "core/memory.h"
#include "semantics/option.h"
#include "util/string.h"  // for str_equals, etc. if needed

/*
    str_join.h — Safe string joining utilities

    All functions:
      - Never allocate unless explicitly named "alloc"
      - Never take ownership
      - Never mutate input unless explicitly stated
      - Return explicit success/failure
*/

CANON_C_DEFINE_OPTION(char*)

/* ============================================================
   Buffer-based join (no allocation, no input mutation)
   ============================================================ */

/*
    str_join:
      Joins `count` strings from `parts` using `sep` into `dest`.
      - sep = NULL treated as empty separator
      - count = 0 produces empty string
      - All parts[i] must be valid null-terminated strings
    Returns:
      true  — success (string fits)
      false — insufficient dest_size or invalid input
*/
static inline bool str_join(
    char* dest,
    size_t dest_size,
    const char** parts,
    size_t count,
    const char* sep
)
{
    if (!dest || dest_size == 0) return false;
    if (count == 0) {
        dest[0] = '\0';
        return true;
    }
    if (!parts) return false;

    sep = sep ? sep : "";
    const size_t sep_len = strlen(sep);

    size_t pos = 0;

    for (size_t i = 0; i < count; ++i) {
        if (!parts[i]) return false;

        const size_t part_len = strlen(parts[i]);

        // Check space for part + separator (except after last) + null
        const size_t needed = part_len + (i + 1 < count && sep_len > 0 ? sep_len : 0) + 1;
        if (pos + needed > dest_size) return false;

        mem_copy(dest + pos, parts[i], part_len);
        pos += part_len;

        if (i + 1 < count && sep_len > 0) {
            mem_copy(dest + pos, sep, sep_len);
            pos += sep_len;
        }
    }

    dest[pos] = '\0';
    return true;
}

/* ============================================================
   Allocating join (caller must free on Some)
   ============================================================ */

/*
    str_alloc_join:
      Allocates and joins `count` strings with `sep`.
      Returns option_charp:
        Some(ptr) — success, caller owns ptr
        None      — allocation failure or invalid input
*/
static inline option_charp str_alloc_join(
    const char** parts,
    size_t count,
    const char* sep
)
{
    if (!parts || count == 0) {
        return str_alloc_copy("");
    }

    sep = sep ? sep : "";
    const size_t sep_len = strlen(sep);

    // Compute total length
    size_t total = 0;
    for (size_t i = 0; i < count; ++i) {
        if (!parts[i]) return option_charp_none();
        total += strlen(parts[i]);
        if (i + 1 < count && sep_len > 0) total += sep_len;
    }
    total += 1;  // null terminator

    char* out = (char*)mem_alloc(total);
    if (!out) return option_charp_none();

    if (str_join(out, total, parts, count, sep)) {
        return option_charp_some(out);
    }

    mem_free(out);
    return option_charp_none();
}

/* ============================================================
   Non-mutating split + join (safe round-trip)
   ============================================================ */

/*
    str_split_to_parts:
      Splits `s` by `delim` into `out_parts` buffer.
      - Does NOT modify `s`
      - Skips empty segments (multiple delimiters treated as one)
      - Stops at max_parts
    Returns: number of parts found
*/
static inline size_t str_split_to_parts(
    const char* s,
    char delim,
    const char** out_parts,
    size_t max_parts
)
{
    if (!s || !out_parts || max_parts == 0) return 0;

    size_t count = 0;
    const char* start = s;

    while (*start) {
        // Skip leading delimiters
        while (*start == delim) ++start;
        if (*start == '\0') break;

        if (count < max_parts) {
            out_parts[count++] = start;
        }

        // Find next delimiter
        while (*start && *start != delim) ++start;

        if (*start == '\0' && count < max_parts) {
            // Last part already added
            break;
        }
    }

    return count;
}

/*
    str_rejoin:
      Convenience: split `s` by `delim`, join with `sep` into `dest`
      - No mutation of `s`
      - Safe and bounded
*/
static inline bool str_rejoin(
    const char* s,
    char delim,
    const char** parts_buffer,
    size_t max_parts,
    char* dest,
    size_t dest_size,
    const char* sep
)
{
    size_t count = str_split_to_parts(s, delim, parts_buffer, max_parts);
    return str_join(dest, dest_size, parts_buffer, count, sep);
}

#endif /* CANON_C_UTIL_STR_JOIN_H */
