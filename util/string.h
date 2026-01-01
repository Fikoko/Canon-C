#ifndef CANON_C_UTIL_STRING_H
#define CANON_C_UTIL_STRING_H

#include <stddef.h>
#include <string.h>
#include <canon/core/memory.h>
#include <canon/semantics/option.h>

/*
    string.h — explicit string helpers (Canon-C Option-based)

    Ownership rules are encoded in function names.

    Functions that allocate:
      - return Option<char*> to indicate success/failure
      - must be freed by the caller

    Functions that operate on buffers:
      - never allocate
      - never free
      - return bool for success/failure
*/

/* ============================================================
   Option type for allocated strings
   ============================================================ */
CANON_C_DEFINE_OPTION(char*)

/* ============================================================
   Owned strings (explicit allocation)
   ============================================================ */

/* Allocate and copy string */
static inline Option_char_ptr str_alloc_copy(const char *s) {
    if (!s) return Option_char_ptr_None();

    size_t len = strlen(s);
    char *out = (char*)mem_alloc(len + 1);
    if (!out) return Option_char_ptr_None();

    mem_copy(out, s, len + 1);
    return Option_char_ptr_Some(out);
}

/* Allocate and concatenate two strings */
static inline Option_char_ptr str_alloc_concat(const char *a, const char *b) {
    if (!a || !b) return Option_char_ptr_None();

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);

    char *out = (char*)mem_alloc(len_a + len_b + 1);
    if (!out) return Option_char_ptr_None();

    mem_copy(out, a, len_a);
    mem_copy(out + len_a, b, len_b + 1);
    return Option_char_ptr_Some(out);
}

/* Allocate substring [start, start+len) */
static inline Option_char_ptr str_alloc_sub(
    const char *s,
    size_t start,
    size_t len
) {
    if (!s) return Option_char_ptr_None();

    size_t s_len = strlen(s);
    if (start >= s_len) return Option_char_ptr_None();

    if (start + len > s_len)
        len = s_len - start;

    char *out = (char*)mem_alloc(len + 1);
    if (!out) return Option_char_ptr_None();

    mem_copy(out, s + start, len);
    out[len] = '\0';
    return Option_char_ptr_Some(out);
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
    if (!dest || !src || dest_size == 0) return false;

    size_t len = strlen(src);
    if (len + 1 > dest_size) return false;

    mem_copy(dest, src, len + 1);
    return true;
}

/* Concatenate strings into caller-provided buffer */
static inline bool str_concat_into(
    char       *dest,
    size_t      dest_size,
    const char *a,
    const char *b
) {
    if (!dest || !a || !b || dest_size == 0) return false;

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);

    if (len_a + len_b + 1 > dest_size) return false;

    mem_copy(dest, a, len_a);
    mem_copy(dest + len_a, b, len_b + 1);
    dest[len_a + len_b] = '\0';
    return true;
}

#endif /* CANON_C_UTIL_STRING_H */
