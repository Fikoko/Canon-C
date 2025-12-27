#ifndef CANON_C_CORE_ARENA_H
#define CANON_C_CORE_ARENA_H

#include <stddef.h>
#include <stdlib.h>

typedef struct Arena {
    unsigned char* buffer;
    size_t capacity;
    size_t offset;
} Arena;

// Initialize arena with given capacity
static inline Arena arena_init(size_t capacity) {
    Arena a;
    a.buffer = (unsigned char*)malloc(capacity);
    a.capacity = capacity;
    a.offset = 0;
    return a;
}

// Allocate memory from arena
static inline void* arena_alloc(Arena* a, size_t size) {
    if (a->offset + size > a->capacity) return NULL;
    void* ptr = a->buffer + a->offset;
    a->offset += size;
    return ptr;
}

// Reset arena (does not free memory)
static inline void arena_reset(Arena* a) {
    a->offset = 0;
}

// Free arena memory
static inline void arena_free(Arena* a) {
    free(a->buffer);
    a->buffer = NULL;
    a->capacity = 0;
    a->offset = 0;
}

#endif // CANON_C_CORE_ARENA_H
