#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>

/*
    map.h — canonical mapping primitive over pointer sequences

    PURPOSE
    -------
    Defines the irreducible "map" operation:
      - transform each element of a pointer sequence
      - without allocating
      - without owning memory
      - without assuming container type

    OWNERSHIP
    ----------
    - Input pointers are borrowed
    - Output pointers are written, not owned
    - This module never allocates or frees memory

    FAILURE
    --------
    - Mapping functions may return NULL
    - NULL values are propagated verbatim
    - No internal error handling is performed

    DEPENDENCIES
    -------------
    None (pure behavioral primitive)
*/

/* Transformation function: input pointer → output pointer */
typedef void* (*MapFn)(void*);

/* ------------------------------------------------------------
   In-place mapping
   Mutates the provided array
   ------------------------------------------------------------ */
static inline void map_inplace(
    void** items,
    size_t count,
    MapFn fn
) {
    if (!items || !fn) return;

    for (size_t i = 0; i < count; i++) {
        items[i] = fn(items[i]);
    }
}

/* ------------------------------------------------------------
   Map into caller-provided output buffer
   Input and output may alias
   ------------------------------------------------------------ */
static inline void map_to(
    void** input,
    size_t count,
    MapFn fn,
    void** output
) {
    if (!input || !output || !fn) return;

    for (size_t i = 0; i < count; i++) {
        output[i] = fn(input[i]);
    }
}

#endif /* CANON_C_ALGO_MAP_H */
