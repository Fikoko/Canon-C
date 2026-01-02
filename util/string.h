#ifndef CANON_C_UTIL_STRING_H
#define CANON_C_UTIL_STRING_H

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "core/memory.h"
#include "semantics/option.h"

/*
    string.h — Explicit, safe string utilities

    Ownership rules (strictly followed):
      - Functions with "alloc" in name: return option_charp
        → Caller owns the pointer on Some, must free it
      - All other functions: operate on caller-provided buffers
        → No allocation, no ownership transfer

    All operations are bounded and null-safe.
    Uses Option<char*> for allocation failure.
*/

CANON_C_DEFINE_OPTION(char*)

/* ============================================================
   Owned strings — allocating functions (caller must free)
   ============================================================ */

/* Allocate and copy a string */
static inline option_charp str_alloc_copy(const char* s)
{
    if (!s) return option_charp_none();

    const size_t len = strlen(s);
    char* out = (char*)mem_alloc(len + 1);
    if (!out) return option_charp_none();

    mem_copy(out, s, len + 1);
    return option_charp_some(out);
}

/* Allocate and concatenate two strings */
static inline option_charp str_alloc_concat(const char* a, const char* b)
{
    if (!a || !b) return option_charp_none();

    const size_t len_a = strlen(a);
    const size_t len_b = strlen(b);
    const size_t total = len_a + len_b;

    char* out = (char*)mem_alloc(total + 1);
    if (!out) return option_charp_none();

    mem_copy(out, a, len_a);
    mem_copy(out + len_a, b, len_b + 1);  // copies null terminator
    return option_charp_some(out);
}

/* Allocate substring: s[start .. start+len) */
static inline option_charp str_alloc_sub(const char* s, size_t start, size_t len)
{
    if (!s) return option_charp_none();

    const size_t s_len = strlen(s);
    if (start >= s_len) return option_charp_none();
    if (start + len > s_len) len = s_len - start;

    char* out = (char*)mem_alloc(len + 1);
    if (!out) return option_charp_none();

    mem_copy(out, s + start, len);
    out[len] = '\0';
    return option_charp_some(out);
}

/* Explicit free helper — reminds caller of ownership */
static inline void str_free(char* s)
{
    mem_free(s);
}

/* ============================================================
   Borrowed strings — buffer-based operations (no allocation)
   ============================================================ */

/* Safe copy into fixed buffer */
static inline bool str_copy_into(char* dest, size_t dest_size, const char* src)
{
    if (!dest || dest_size == 0 || !src) return false;

    const size_t len = strlen(src);
    if (len + 1 > dest_size) return false;

    mem_copy(dest, src, len + 1);
    return true;
}

/* Safe concatenation into fixed buffer */
static inline bool str_concat_into(char* dest, size_t dest_size, const char* a, const char* b)
{
    if (!dest || dest_size == 0 || !a || !b) return false;

    const size_t len_a = strlen(a);
    const size_t len_b = strlen(b);
    if (len_a + len_b + 1 > dest_size) return false;

    mem_copy(dest, a, len_a);
    mem_copy(dest + len_a, b, len_b + 1);
    return true;
}

/* ============================================================
   Common predicates (pure, no allocation)
   ============================================================ */

static inline bool str_equals(const char* a, const char* b)
{
    if (a == b) return true;
    if (!a || !b) return false;
    return strcmp(a, b) == 0;
}

static inline bool str_starts_with(const char* s, const char* prefix)
{
    if (!s || !prefix) return false;
    const size_t plen = strlen(prefix);
    return plen == 0 || strncmp(s, prefix, plen) == 0;
}

static inline bool str_ends_with(const char* s, const char* suffix)
{
    if (!s || !suffix) return false;
    const size_t slen = strlen(s);
    const size_t flen = strlen(suffix);
    if (flen > slen) return false;
    return strcmp(s + slen - flen, suffix) == 0;
}

#endif /* CANON_C_UTIL_STRING_H */
