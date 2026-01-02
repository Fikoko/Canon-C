#ifndef CANON_C_CORE_MEMORY_H
#define CANON_C_CORE_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================
   Optional features
   ============================================================ */
#define MEM_DEBUG      1   /* Enable debug output */
#define MEM_STATS      1   /* Enable allocation statistics */

/* ============================================================
   Ownership semantics
   ============================================================ */
typedef enum { MEM_OWNED, MEM_BORROWED } MemOwnership;

/*
   Memory block wrapper for ownership-tracking and safety.
*/
typedef struct MemBlock {
    void* ptr;
    size_t size;
    MemOwnership ownership;
} MemBlock;

/* ============================================================
   Basic heap allocation
   ============================================================ */

/*
   mem_alloc(size):
   Allocates `size` bytes from the heap.
   Ownership: caller owns the memory and must free.
   Failure: returns NULL if allocation fails or size==0.
*/
static inline void* mem_alloc(size_t size) {
    if (size == 0) return NULL;
    void* ptr = malloc(size);
#if MEM_STATS
    /* optional stats tracking */
#endif
    return ptr;
}

/*
   mem_free(ptr):
   Frees previously allocated memory. Safe for NULL.
*/
static inline void mem_free(void* ptr) {
    if (!ptr) return;
    free(ptr);
}

/*
   mem_realloc(ptr, new_size):
   Resizes a previously allocated memory block.
   If new_size == 0, frees ptr and returns NULL.
*/
static inline void* mem_realloc(void* ptr, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, new_size);
}

/*
   mem_alloc_zero(size):
   Allocates and zero-initializes memory.
   Ownership: caller owns memory.
   Returns NULL on failure or size==0.
*/
static inline void* mem_alloc_zero(size_t size) {
    if (size == 0) return NULL;
    return calloc(1, size);
}

/* ============================================================
   Memory operations
   ============================================================ */

/* Copy memory; no overlap allowed */
static inline void mem_copy(void* dest, const void* src, size_t size) {
    if (!dest || !src || size == 0) return;
    memcpy(dest, src, size);
}

/* Move memory; overlap allowed */
static inline void mem_move(void* dest, const void* src, size_t size) {
    if (!dest || !src || size == 0) return;
    memmove(dest, src, size);
}

/* Zero memory */
static inline void mem_zero(void* ptr, size_t size) {
    if (!ptr || size == 0) return;
    memset(ptr, 0, size);
}

/* Set memory to a specific byte value */
static inline void mem_set(void* ptr, int value, size_t size) {
    if (!ptr || size == 0) return;
    memset(ptr, value, size);
}

/* Compare memory regions; 0 if equal */
static inline int mem_compare(const void* a, const void* b, size_t size) {
    if (!a || !b || size == 0) return 0;
    return memcmp(a, b, size);
}

/* ============================================================
   Alignment helper
   ============================================================ */

/* Align size to nearest multiple of max_align_t */
static inline size_t mem_align(size_t size) {
    size_t align = sizeof(max_align_t);
    if (size > SIZE_MAX - (align - 1)) return SIZE_MAX;
    return (size + align - 1) & ~(align - 1);
}

/* ============================================================
   Typed allocation macros
   ============================================================ */
#define mem_alloc_type(Type) ((Type*)mem_alloc(sizeof(Type)))
#define mem_alloc_array(Type,count) ((Type*)mem_alloc(sizeof(Type)*(count)))
#define mem_alloc_type_zero(Type) ({ Type* p = mem_alloc_type(Type); if(p) memset(p,0,sizeof(Type)); p; })
#define mem_alloc_array_zero(Type,count) ({ Type* p = mem_alloc_array(Type,count); if(p) memset(p,0,sizeof(Type)*(count)); p; })

#endif /* CANON_C_CORE_MEMORY_H */
