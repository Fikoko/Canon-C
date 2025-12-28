#ifndef CANON_C_CORE_MEMORY_H
#define CANON_C_CORE_MEMORY_H

/*
    memory.h — Canon-C core memory primitives

    Explicit, minimal memory mechanisms.
    No hidden ownership.
    No implicit lifetime.
    No policy.
*/

#include <stddef.h>   /* size_t */
#include <stdlib.h>   /* malloc, free, realloc */
#include <string.h>   /* memcpy, memmove, memset */

/* ============================================================
   Basic allocation
   ============================================================ */

/* Allocate a block of memory.
   Returns NULL on failure or size == 0. */
static inline void *mem_alloc(size_t size) {
    if (size == 0) return NULL;
    return malloc(size);
}

/* Free a previously allocated block.
   Accepts NULL. */
static inline void mem_free(void *ptr) {
    free(ptr);
}

/* Resize a memory block.
   Returns NULL on failure (original block unchanged). */
static inline void *mem_realloc(void *ptr, size_t new_size) {
    if (new_size == 0) return NULL;
    return realloc(ptr, new_size);
}

/* Allocate zero-initialized memory */
static inline void *mem_alloc_zero(size_t size) {
    if (size == 0) return NULL;
    void *ptr = malloc(size);
    if (!ptr) return NULL;
    memset(ptr, 0, size);
    return ptr;
}

/* ============================================================
   Memory operations
   ============================================================ */

/* Copy memory (no overlap allowed). */
static inline void mem_copy(void *dest, const void *src, size_t size) {
    if (size == 0) return;
    memcpy(dest, src, size);
}

/* Move memory (overlap allowed). */
static inline void mem_move(void *dest, const void *src, size_t size) {
    if (size == 0) return;
    memmove(dest, src, size);
}

/* Set memory to zero. */
static inline void mem_zero(void *ptr, size_t size) {
    if (size == 0) return;
    memset(ptr, 0, size);
}

/* Set memory to a byte value. */
static inline void mem_set(void *ptr, int value, size_t size) {
    if (size == 0) return;
    memset(ptr, value, size);
}

/* Compare memory regions.
   Returns 0 if equal (memcmp semantics). */
static inline int mem_compare(const void *a, const void *b, size_t size) {
    if (size == 0) return 0;
    return memcmp(a, b, size);
}

/* ============================================================
   Intent helpers
   ============================================================ */

/* Align size to pointer width */
static inline size_t mem_align(size_t size) {
    size_t align = sizeof(void *);
    return (size + align - 1) & ~(align - 1);
}

#endif /* CANON_C_CORE_MEMORY_H */
