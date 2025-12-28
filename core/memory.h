#ifndef CANON_CORE_MEMORY_H
#define CANON_CORE_MEMORY_H

/*
    memory.h — Canon-C core memory primitives

    This header defines explicit, minimal mechanisms for memory allocation,
    lifetime control, and memory manipulation.

    It introduces NO ownership magic, NO garbage collection,
    and NO hidden behavior.

    Memory is treated as a physical resource with explicit lifetime.
*/

#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uint8_t */

/* ============================================================
   Basic allocation
   ============================================================ */

/* Allocate a block of memory.
   Returns NULL on failure. */
void *mem_alloc(size_t size);

/* Free a previously allocated block.
   Accepts NULL. */
void mem_free(void *ptr);

/* Resize a memory block.
   Returns NULL on failure (original block is unchanged). */
void *mem_realloc(void *ptr, size_t new_size);


/* ============================================================
   Memory operations
   ============================================================ */

/* Copy memory (no overlap allowed). */
void mem_copy(void *dest, const void *src, size_t size);

/* Move memory (overlap allowed). */
void mem_move(void *dest, const void *src, size_t size);

/* Set memory to zero. */
void mem_zero(void *ptr, size_t size);


/* ============================================================
   Arena allocator (explicit lifetime)
   ============================================================ */

/*
    Arena represents a linear allocation region.

    - Allocations are fast
    - Individual frees are not allowed
    - Lifetime is controlled explicitly by reset/free
*/

typedef struct Arena {
    uint8_t *buffer;
    size_t   capacity;
    size_t   offset;
} Arena;

/* Initialize an arena with a given backing buffer. */
void arena_init(Arena *arena, void *buffer, size_t capacity);

/* Allocate memory from the arena.
   Returns NULL if capacity is exceeded. */
void *arena_alloc(Arena *arena, size_t size);

/* Reset arena allocation offset.
   Does NOT free the backing buffer. */
void arena_reset(Arena *arena);


/* ============================================================
   Safety & intent helpers (optional but explicit)
   ============================================================ */

/* Align size to pointer width. */
size_t mem_align(size_t size);

/* Check whether an arena has enough remaining space. */
int arena_can_alloc(const Arena *arena, size_t size);

#endif /* CANON_CORE_MEMORY_H */
