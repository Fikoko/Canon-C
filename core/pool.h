#ifndef CANON_C_CORE_POOL_H
#define CANON_C_CORE_POOL_H

#include <stddef.h>
#include <stdbool.h>
#include "arena.h"

/*
    pool.h — Fixed-size object pool allocator

    Fast, deterministic allocation for objects of one size.
    Backed by Arena (linear bump within fixed buffer).
    No individual frees — only full reset.

    Ideal for nodes, temporary objects, etc.
*/

typedef struct {
    Arena* arena;       // backing arena (caller-owned)
    size_t object_size; // aligned size of each object
    size_t capacity;    // max objects
    size_t used;        // objects currently allocated
} Pool;

/* Initialize pool with pre-allocated arena buffer */
static inline bool pool_init(Pool* pool, Arena* arena, size_t object_size, size_t max_objects)
{
    if (!pool || !arena || object_size == 0 || max_objects == 0) return false;

    size_t aligned_size = mem_align(object_size);
    size_t needed = aligned_size * max_objects;

    if (needed > arena_remaining(arena)) return false;

    pool->arena = arena;
    pool->object_size = aligned_size;
    pool->capacity = max_objects;
    pool->used = 0;

    return true;
}

/* Allocate one object — returns aligned pointer or NULL */
static inline void* pool_alloc(Pool* pool)
{
    if (!pool || pool->used >= pool->capacity) return NULL;

    void* ptr = arena_alloc(pool->arena, pool->object_size);
    if (ptr) pool->used++;
    return ptr;
}

/* Current usage */
static inline size_t pool_used(const Pool* pool) { return pool ? pool->used : 0; }
static inline size_t pool_capacity(const Pool* pool) { return pool ? pool->capacity : 0; }
static inline bool pool_is_full(const Pool* pool) { return pool && pool->used >= pool->capacity; }

/* Reset entire pool (invalidates all objects) */
static inline void pool_reset(Pool* pool)
{
    if (!pool || !pool->arena) return;
    ArenaMark mark = arena_mark(pool->arena);
    arena_reset_to(pool->arena, mark - pool->object_size * pool->used);
    pool->used = 0;
}

#endif /* CANON_C_CORE_POOL_H */
