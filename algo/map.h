#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>
#include <stdbool.h>
#include <canon/core/memory.h>

/*
    map.h — functional-style mapping over pointer sequences

    A map operation transforms:
        input[i] -> output[i]

    This module:
    - Does not track ownership
    - Does not free memory
    - Does not assume allocation strategy

    Allocation, if any, is always explicit.
*/

typedef void* (*MapFunc)(void*);

/* ============================================================
   In-place transformation
   ============================================================ */

/* Mutate array by applying f to each element */
static inline void map_inplace(void **items, size_t len, MapFunc f) {
    if (!items || !f) return;

    for (size_t i = 0; i < len; i++) {
        items[i] = f(items[i]);
    }
}

/* ============================================================
   Explicit allocation
   ============================================================ */

/* Allocate output array using core allocator */
static inline bool map_alloc(
    void   **items,
    size_t   len,
    MapFunc  f,
    void ***out_items
) {
    if (!items || !f || !out_items) return false;

    void **out = (void **)mem_alloc(sizeof(void *) * len);
    if (!out) return false;

    for (size_t i = 0; i < len; i++) {
        out[i] = f(items[i]);
    }

    *out_items = out;
    return true;
}

/* ============================================================
   Manual buffer
   ============================================================ */

/* Map into caller-provided output buffer */
static inline bool map_into(
    void   **items,
    size_t   len,
    MapFunc  f,
    void   **output
) {
    if (!items || !output || !f) return false;

    for (size_t i = 0; i < len; i++) {
        output[i] = f(items[i]);
    }

    return true;
}

#endif /* CANON_C_ALGO_MAP_H */
