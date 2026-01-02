#ifndef CANON_C_ALGO_SEARCH_H
#define CANON_C_ALGO_SEARCH_H

#include <stddef.h>

/*
    search.h — Binary search utilities (requires sorted input)

    All functions assume array is sorted by the same comparator.
*/

typedef int (*algo_cmp_fn)(const void* a, const void* b, void* ctx);

/* Find exact match — returns index or SIZE_MAX if not found */
static inline size_t algo_lower_bound(
    const void* array,
    size_t len,
    size_t elem_size,
    const void* key,
    algo_cmp_fn cmp,
    void* ctx
)
{
    size_t low = 0;
    size_t high = len;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void* elem = (const char*)array + mid * elem_size;
        int c = cmp(elem, key, ctx);
        if (c < 0) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    return low < len && cmp((const char*)array + low * elem_size, key, ctx) == 0 ? low : SIZE_MAX;
}

/* Typed macros */
#define ALGO_LOWER_BOUND_TYPED(array, len, Type, key, cmp_expr, ctx) \
    ({ \
        size_t _idx = SIZE_MAX; \
        if ((array)) { \
            _idx = algo_lower_bound((array), (len), sizeof(Type), (key), \
                                    (algo_cmp_fn)(cmp_expr), (ctx)); \
        } \
        _idx; \
    })

#define ALGO_BINARY_SEARCH_TYPED(array, len, Type, key, cmp_expr, ctx) \
    (ALGO_LOWER_BOUND_TYPED(array, len, Type, key, cmp_expr, ctx) != SIZE_MAX)

#endif /* CANON_C_ALGO_SEARCH_H */
