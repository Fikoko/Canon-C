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

typedef struct Arena {
    uint8_t *buffer;
    size_t   capacity;
    size_t   offset;
} Arena;

/* Initialize arena with an existing buffer */
void arena_init(Arena *arena, void *buffer, size_t capacity);

/* Allocate memory from arena */
void *arena_alloc(Arena *arena, size_t size);

/* Reset arena (reuse memory) */
void arena_reset(Arena *arena);

/* Check remaining capacity */
size_t arena_remaining(const Arena *arena);

#endif /* CANON_C_CORE_ARENA_H */
