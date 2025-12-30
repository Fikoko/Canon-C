#ifndef CANON_C_CORE_ARENA_H
#define CANON_C_CORE_ARENA_H

#include <stddef.h>
#include <stdint.h>
#include "memory.h"

/*
    arena.h — linear allocation with explicit lifetime

    Arena does not own memory by default.
    It operates on a caller-provided buffer.
*/

/* Linear allocation arena structure */
typedef struct Arena {
    uint8_t *buffer;
    size_t   capacity;
    size_t   offset;
} Arena;

/* ------------------------------------------------------------
   Initialize arena with an existing buffer
   ------------------------------------------------------------ */
static inline void arena_init(Arena *arena, void *buffer, size_t capacity) {
    arena->buffer = (uint8_t*)buffer;
    arena->capacity = capacity;
    arena->offset = 0;
}

/* ------------------------------------------------------------
   Allocate memory from arena
   Returns NULL if capacity is exceeded
   ------------------------------------------------------------ */
static inline void* arena_alloc(Arena *arena, size_t size) {
    if (!arena || size == 0) return NULL;

    size = mem_align(size);

    if (arena->offset + size > arena->capacity) return NULL;

    void *ptr = arena->buffer + arena->offset;
    arena->offset += size;
    return ptr;
}

/* ------------------------------------------------------------
   Reset arena allocation offset (reuse memory)
   ------------------------------------------------------------ */
static inline void arena_reset(Arena *arena) {
    if (!arena) return;
    arena->offset = 0;
}

/* ------------------------------------------------------------
   Check remaining capacity
   ------------------------------------------------------------ */
static inline size_t arena_remaining(const Arena *arena) {
    if (!arena) return 0;
    return arena->capacity - arena->offset;
}

#endif /* CANON_C_CORE_ARENA_H */
