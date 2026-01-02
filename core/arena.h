#ifndef CANON_C_CORE_ARENA_H
#define CANON_C_CORE_ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "memory.h"

/* ============================================================
   Arena structure
   ============================================================ */

/*
   Linear allocation arena:
   - Allocates sequentially from a caller-provided buffer.
   - No hidden allocation.
   - Lifetime of allocations visible via arena offset.
*/
typedef struct Arena {
    uint8_t* buffer;   /* memory buffer */
    size_t capacity;   /* total bytes available */
    size_t offset;     /* current allocation offset */
} Arena;

/* ============================================================
   Initialize arena
   ============================================================ */

/*
   arena_init(arena, buffer, capacity):
   Initializes an arena with a fixed memory buffer.
   Preconditions: buffer != NULL, capacity > 0.
   Ownership: caller owns buffer.
*/
static inline void arena_init(Arena* arena, void* buffer, size_t capacity) {
    if (!arena || !buffer || capacity == 0) return;
    arena->buffer = (uint8_t*)buffer;
    arena->capacity = capacity;
    arena->offset = 0;
}

/* ============================================================
   Allocate memory
   ============================================================ */

/*
   arena_alloc(arena, size):
   Allocates `size` bytes from the arena.
   Returns NULL if insufficient capacity.
   Ownership: caller may use memory until arena is reset.
*/
static inline void* arena_alloc(Arena* arena, size_t size) {
    if (!arena || size == 0) return NULL;
    size = mem_align(size);
    if (size > arena->capacity - arena->offset) return NULL;
    void* ptr = arena->buffer + arena->offset;
    arena->offset += size;
    return ptr;
}

/*
   arena_alloc_aligned(arena, size, alignment):
   Allocates `size` bytes aligned to `alignment`.
   Returns NULL if insufficient capacity.
*/
static inline void* arena_alloc_aligned(Arena* arena, size_t size, size_t alignment) {
    if (!arena || size == 0) return NULL;
    uintptr_t current = (uintptr_t)(arena->buffer + arena->offset);
    uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned - current;
    if (arena->offset + padding + size > arena->capacity) return NULL;
    arena->offset += padding;
    void* ptr = arena->buffer + arena->offset;
    arena->offset += size;
    return ptr;
}

/* ============================================================
   Reset / checkpoint
   ============================================================ */

/* Reset arena to empty (all allocations invalidated) */
static inline void arena_reset(Arena* arena) {
    if (!arena) return;
    arena->offset = 0;
}

/* Remaining capacity in bytes */
static inline size_t arena_remaining(const Arena* arena) {
    if (!arena) return 0;
    return arena->capacity - arena->offset;
}

/* Arena mark for partial rollback */
typedef size_t ArenaMark;

/* Create a checkpoint of current offset */
static inline ArenaMark arena_mark(Arena* arena) {
    return arena ? arena->offset : 0;
}

/* Reset arena to a previous mark */
static inline void arena_reset_to(Arena* arena, ArenaMark mark) {
    if (!arena || mark > arena->capacity) return;
    arena->offset = mark;
}

/* ============================================================
   Typed allocation macros
   ============================================================ */
#define arena_alloc_type(arena, Type) ((Type*)arena_alloc((arena), sizeof(Type)))
#define arena_alloc_array(arena, Type, count) ((Type*)arena_alloc((arena), sizeof(Type)*(count)))
#define arena_alloc_type_zero(arena, Type) ({ Type* p = arena_alloc_type((arena), Type); if(p) memset(p,0,sizeof(Type)); p; })
#define arena_alloc_array_zero(arena, Type, count) ({ Type* p = arena_alloc_array((arena), Type, count); if(p) memset(p,0,sizeof(Type)*(count)); p; })

#endif /* CANON_C_CORE_ARENA_H */
