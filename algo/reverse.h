#ifndef CANON_C_ALGO_REVERSE_H
#define CANON_C_ALGO_REVERSE_H

#include <stddef.h>
#include "core/memory.h"

/*
    reverse.h — Reverse sequence in-place or into output
*/

static inline void algo_reverse(
    void* array,
    size_t len,
    size_t elem_size
)
{
    if (!array || len < 2) return;
    char* bytes = (char*)array;
    char* left = bytes;
    char* right = bytes + (len - 1) * elem_size;
    char tmp[256];  // safe for reasonable elem_size
    while (left < right) {
        mem_copy(tmp, left, elem_size);
        mem_copy(left, right, elem_size);
        mem_copy(right, tmp, elem_size);
        left += elem_size;
        right -= elem_size;
    }
}

/* Typed macro */
#define ALGO_REVERSE_TYPED(array, len, Type) \
    do { \
        if ((array) && (len) >= 2) { \
            algo_reverse((array), (len), sizeof(Type)); \
        } \
    } while (0)

/* Vec version */
#define ALGO_REVERSE_VEC(vec, Type) \
    ALGO_REVERSE_TYPED((vec).items, (vec).len, Type)

#endif /* CANON_C_ALGO_REVERSE_H */
