#ifndef CANON_C_DATA_STRINGBUF_H
#define CANON_C_DATA_STRINGBUF_H

#include <stddef.h>
#include <stdarg.h>
#include "arena.h"
#include "util/string.h"

/*
    StringBuf — Incremental string builder (arena-backed)

    No ownership transfer.
    All appended data lives until arena reset.
*/

typedef struct {
    Arena* arena;
    char* data;
    size_t len;
    size_t capacity;
} StringBuf;

/* Init with arena (preferred) */
static inline bool stringbuf_init_arena(StringBuf* sb, Arena* arena, size_t initial_cap)
{
    if (!sb || !arena) return false;
    char* buf = arena_alloc(arena, initial_cap);
    if (!buf) return false;
    buf[0] = '\0';
    *sb = (StringBuf){ .arena = arena, .data = buf, .len = 0, .capacity = initial_cap };
    return true;
}

/* Init with fixed buffer (fallback) */
static inline void stringbuf_init_buffer(StringBuf* sb, char* buffer, size_t cap)
{
    if (sb && buffer && cap > 0) {
        buffer[0] = '\0';
        *sb = (StringBuf){ .data = buffer, .len = 0, .capacity = cap };
    }
}

/* Append raw string */
static inline bool stringbuf_append(StringBuf* sb, const char* s)
{
    if (!sb || !s || !sb->data) return false;
    size_t add_len = strlen(s);
    if (sb->len + add_len + 1 > sb->capacity) return false;
    mem_copy(sb->data + sb->len, s, add_len + 1);
    sb->len += add_len;
    return true;
}

/* Append formatted */
static inline bool stringbuf_append_fmt(StringBuf* sb, const char* fmt, ...)
{
    if (!sb || !fmt) return false;
    va_list args;
    va_start(args, fmt);
    int needed = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (needed < 0 || sb->len + needed + 1 > sb->capacity) return false;

    va_start(args, fmt);
    vsnprintf(sb->data + sb->len, sb->capacity - sb->len, fmt, args);
    va_end(args);
    sb->len += needed;
    return true;
}

/* Get final string (borrowed) */
static inline const char* stringbuf_str(const StringBuf* sb)
{
    return sb && sb->data ? sb->data : "";
}

static inline size_t stringbuf_len(const StringBuf* sb) { return sb ? sb->len : 0; }

#endif /* CANON_C_DATA_STRINGBUF_H */
