#ifndef CANON_C_ALGO_FOLD_H
#define CANON_C_ALGO_FOLD_H

#include <stddef.h>

/*
    fold.h — aggregate elements into a single value

    Semantics:
    - Left-to-right fold
    - Explicit accumulator
    - No allocation
    - No ownership
*/

/*
    Fold function:
    - acc  : accumulator (caller-owned)
    - item : current element
    - ctx  : optional user context (may be NULL)
*/
typedef void (*FoldFn)(void *acc, void *item, void *ctx);

/*
    Fold items into accumulator.

    Requirements:
    - acc must point to valid, initialized storage
*/
static inline void fold(
    void   *acc,
    void  **items,
    size_t  len,
    FoldFn  f,
    void   *ctx
) {
    if (!acc || !items || !f) return;

    for (size_t i = 0; i < len; i++) {
        f(acc, items[i], ctx);
    }
}

#endif /* CANON_C_ALGO_FOLD_H */
