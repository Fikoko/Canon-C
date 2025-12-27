#ifndef CANON_C_UTIL_STRING_H
#define CANON_C_UTIL_STRING_H

#include <stdlib.h>
#include <string.h>

// --- Automatic mode ---
static inline char* str_dup(const char* s) {
    size_t len = strlen(s);
    char* copy = (char*)malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

static inline char* str_concat(const char* a, const char* b) {
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    char* new_str = (char*)malloc(len_a + len_b + 1);
    if (!new_str) return NULL;
    memcpy(new_str, a, len_a);
    memcpy(new_str + len_a, b, len_b + 1);
    return new_str;
}

static inline char* str_sub(const char* s, size_t start, size_t len) {
    size_t s_len = strlen(s);
    if (start >= s_len) return NULL;
    if (start + len > s_len) len = s_len - start;
    char* sub = (char*)malloc(len + 1);
    if (!sub) return NULL;
    memcpy(sub, s + start, len);
    sub[len] = '\0';
    return sub;
}

// --- Manual mode ---

// Copy string into preallocated buffer
static inline bool str_copy_manual(char* dest, size_t dest_size, const char* src) {
    size_t len = strlen(src);
    if (len + 1 > dest_size) return false;
    memcpy(dest, src, len + 1);
    return true;
}

// Concatenate strings into preallocated buffer
static inline bool str_concat_manual(char* dest, size_t dest_size, const char* a, const char* b) {
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    if (len_a + len_b + 1 > dest_size) return false;
    memcpy(dest, a, len_a);
    memcpy(dest + len_a, b, len_b + 1);
    dest[len_a + len_b] = '\0';
    return true;
}

#endif // CANON_C_UTIL_STRING_H
