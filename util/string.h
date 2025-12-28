#ifndef CANON_C_UTIL_STRING_H
#define CANON_C_UTIL_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
    string.h — basic string utilities

    Automatic mode:
      - Allocates using malloc
      - Caller owns returned memory

    Manual mode:
      - Operates on caller-provided buffers
      - Never allocates
*/

// =====================
// Automatic mode
// =====================

// Duplicate string (malloc)
static inline char* str_dup(const char* s) {
    if (!s) return NULL;

    size_t len = strlen(s);
    char* copy = (char*)malloc(len + 1);
    if (!copy) return NULL;

    memcpy(copy, s, len + 1);
    return copy;
}

// Concatenate two strings (malloc)
static inline char* str_concat(const char* a, const char* b) {
    if (!a || !b) return NULL;

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);

    char* out = (char*)malloc(len_a + len_b + 1);
    if (!out) return NULL;

    memcpy(out, a, len_a);
    memcpy(out + len_a, b, len_b + 1);
    return out;
}

// Substring (malloc)
static inline char* str_sub(const char* s, size_t start, size_t len) {
    if (!s) return NULL;

    size_t s_len = strlen(s);
    if (start >= s_len) return NULL;

    if (start + len > s_len)
        len = s_len - start;

    char* out = (char*)malloc(len + 1);
    if (!out) return NULL;

    memcpy(out, s + start, len);
    out[len] = '\0';
    return out;
}

// =====================
// Manual mode
// =====================

// Copy string into preallocated buffer
static inline bool str_copy_manual(
    char* dest,
    size_t dest_size,
    const char* src
) {
    if (!dest || !src) return false;

    size_t len = strlen(src);
    if (len + 1 > dest_size) return false;

    memcpy(dest, src, len + 1);
    return true;
}

// Concatenate into preallocated buffer
static inline bool str_concat_manual(
    char* dest,
    size_t dest_size,
    const char* a,
    const char* b
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
