#ifndef CANON_C_ALGO_UNIQUE_H
#define CANON_C_ALGO_UNIQUE_H

#include <stddef.h>
#include "data/vec.h"

/*
    unique.h — Remove consecutive or all duplicates

    Requires sorted input for full deduplication.
    Preserves order of first occurrence.
*/

typedef int (*algo_cmp_fn)(const void* a, const void* b, void* ctx);

/* Remove consecutive duplicates in-place */
static inline size_t algo_unique_consecutive(
    void* array,
    size_t len,
    size_t elem_size,
    algo_cmp_fn cmp,
    void* ctx
)
{
    if (!array || len <= 1) return len;

    char* bytes = (char*)array;
    size_t write = 1;

    for (size_t read = 1; read < len; ++read) {
        const void* prev = bytes + (write - 1) * elem_size;
        const void* curr = bytes + read * elem_size;
        if (cmp(prev, curr, ctx) != 0) {
            if (write != read) {
                mem_copy(bytes + write * elem_size, curr, elem_size);
            }
            ++write;
        }
    }
    return write;
}

/* Typed macro for consecutive unique */
#define ALGO_UNIQUE_CONSECUTIVE_TYPED(array, len, Type, cmp_expr, ctx) \
    ({ \
        size_t _new_len = (len); \
        if ((array)) { \
            _new_len = algo_unique_consecutive((array), (len), sizeof(Type), \
                                               (algo_cmp_fn)(cmp_expr), (ctx)); \
        } \
        _new_len; \
    })

/* Vec version */
#define ALGO_UNIQUE_CONSECUTIVE_VEC(vec, Type, cmp_expr, ctx) \
    do { \
        if ((vec).items) { \
            size_t _new_len = algo_unique_consecutive((vec).items, (vec).len, sizeof(Type), \
                                                      (algo_cmp_fn)(cmp_expr), (ctx)); \
            (vec).len = _new_len; \
        } \
    } while (0)

#endif /* CANON_C_ALGO_UNIQUE_H */
