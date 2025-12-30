#ifndef CANON_C_UTIL_STRING_H
#define CANON_C_UTIL_STRING_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <canon/core/memory.h>

/*
    string.h — explicit string helpers

    Ownership rules are encoded in function names.

    Functions that allocate:
      - return owned memory
      - must be freed by the caller

    Functions that operate on buffers:
      - never allocate
      - never free
*/

/* ============================================================
   Owned strings (explicit allocation)
   ============================================================ */

/* Allocate and copy string */
static inline char *str_alloc_copy(const char *s) {
    if (!s) return NULL;

    size_t len = strlen(s);
    char *out = (char *)mem_alloc(len + 1);
    if (!out) return NULL;

    memcpy(out, s, len + 1);
    return out;
}

/* Allocate and concatenate two strings */
static inline char *str_alloc_concat(const char *a, const char *b) {
    if (!a || !b) return NULL;

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);

    char *out = (char *)mem_alloc(len_a + len_b + 1);
    if (!out) return NULL;

    memcpy(out, a, len_a);
    memcpy(out + len_a, b, len_b + 1);
    return out;
}

/* Allocate substring */
static inline char *str_alloc_sub(
    const char *s,
    size_t start,
    size_t len
) {
    if (!s) return NULL;

    size_t s_len = strlen(s);
    if (start >= s_len) return NULL;

    if (start + len > s_len)
        len = s_len - start;

    char *out = (char *)mem_alloc(len + 1);
    if (!out) return NULL;

    memcpy(out, s + start, len);
    out[len] = '\0';
    return out;
}

/* ============================================================
   Buffer-based operations (no allocation)
   ============================================================ */

/* Copy string into caller-provided buffer */
static inline bool str_copy_into(
    char       *dest,
    size_t      dest_size,
    const char *src
) {
    if (!dest || !src) return false;

    size_t len = strlen(src);
    if (len + 1 > dest_size) return false;

    memcpy(dest, src, len + 1);
    return true;
}

/* Concatenate strings into caller-provided buffer */
static inline bool str_concat_into(
    char       *dest,
    size_t      dest_size,
    const char *a,
    const char *b
) {
    if (!dest || !a || !b) return false;

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);

    if (len_a + len_b + 1 > dest_size)
        return false;

    memcpy(dest, a, len_a);
    memcpy(dest + len_a, b, len_b + 1);
    return true;
}

#endif /* CANON_C_UTIL_STRING_H */
