#ifndef CANON_C_CORE_MEMORY_H
#define CANON_C_CORE_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ============================================================
   Alignment
   ============================================================ */

/*
   mem_align(size):
   Aligns `size` upwards to the natural alignment boundary
   (sizeof(max_align_t)), suitable for most allocation needs.
   Safe against overflow.
*/
static inline size_t mem_align(size_t size)
{
    const size_t align = sizeof(max_align_t);
    /* Prevent overflow in addition */
    if (size > SIZE_MAX - (align - 1u)) return SIZE_MAX;
    return (size + align - 1u) & ~(align - 1u);
}

/*
   mem_align_to(size, alignment):
   Aligns `size` upwards to a specific power-of-two alignment.
   Alignment must be a power of two.
*/
static inline size_t mem_align_to(size_t size, size_t alignment)
{
    if (size > SIZE_MAX - (alignment - 1u)) return SIZE_MAX;
    return (size + alignment - 1u) & ~(alignment - 1u);
}

/* ============================================================
   Safe memory operations
   ============================================================ */

/* Copy non-overlapping memory */
static inline void mem_copy(void* dest, const void* src, size_t size)
{
    if (dest && src && size != 0) {
        memcpy(dest, src, size);
    }
}

/* Move memory (handles overlap) */
static inline void mem_move(void* dest, const void* src, size_t size)
{
    if (dest && src && size != 0) {
        memmove(dest, src, size);
    }
}

/* Zero-fill memory */
static inline void mem_zero(void* ptr, size_t size)
{
    if (ptr && size != 0) {
        memset(ptr, 0, size);
    }
}

/* Fill memory with a byte value */
static inline void mem_set(void* ptr, int value, size_t size)
{
    if (ptr && size != 0) {
        memset(ptr, value, size);
    }
}

/* Compare memory regions (-1, 0, +1 like memcmp) */
static inline int mem_compare(const void* a, const void* b, size_t size)
{
    if (!a || !b || size == 0) return 0;
    return memcmp(a, b, size);
}

#endif /* CANON_C_CORE_MEMORY_H */
