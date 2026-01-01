#ifndef CANON_C_ALGO_FILTER_H
#define CANON_C_ALGO_FILTER_H

#include <stddef.h>
#include <stdbool.h>

/*
    filter.h — select elements based on predicate

    This is a derived utility built on top of canonical primitives.

    - No allocation policy is imposed
    - No ownership is assumed
    - No side effects beyond writing output
    - Predicate must be pure from the caller’s perspective
*/

/* Predicate over elements */
typedef bool (*FilterPred)(void *item);

/* ============================================================
   Manual mode (preferred)
   ============================================================ */

/*
    Filters items into caller-provided output buffer.

    Returns:
    - number of elements written to output

    Requirements:
    - output must have capacity >= len
*/
static inline size_t filter_manual(
    void **items,
    size_t len,
    FilterPred pred,
    void **output
) {
    if (!items || !pred || !output) return 0;

    size_t out_len = 0;

    for (size_t i = 0; i < len; i++) {
        if (pred(items[i])) {
            output[out_len++] = items[i];
        }
    }

    return out_len;
}

#endif /* CANON_C_ALGO_FILTER_H */

