#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>
#include <stdlib.h>

/*
    map.h — functional-style mapping over pointer arrays

    MapFunc defines a transformation:
      - Input pointer -> Output pointer
      - May allocate, reuse, or return NULL
      - Ownership rules are defined by the caller

    This module does NOT:
      - Track ownership
      - Free memory
      - Assume allocation strategy
*/

typedef void* (*MapFunc)(void*);

// =====================
// Automatic mode
// =====================

// In-place map (mutates array)
static inline void map_inplace(void** items, size_t len, MapFunc f) {
    if (!items || !f) return;

    for (size_t i = 0; i < len; i++) {
        items[i] = f(items[i]);
    }
}

// Allocate new array and map into it
static inline void** map_new(void** items, size_t len, MapFunc f) {
    if (!items || !f) return NULL;

    void** out = (void**)malloc(sizeof(void*) * len);
    if (!out) return NULL;

    for (size_t i = 0; i < len; i++) {
        out[i] = f(items[i]);
    }

    return out;
}

// =====================
// Manual mode
// =====================

// Map into caller-provided output buffer
static inline void map_manual(
    void** items,
    size_t len,
    MapFunc f,
    void** output
) {
    if (!items || !output || !f) return;

    for (size_t i = 0; i < len; i++) {
        output[i] = f(items[i]);
    }
}

#endif /* CANON_C_ALGO_MAP_H */
