#ifndef CANON_C_ALGO_FOLD_H
#define CANON_C_ALGO_FOLD_H

#include <stddef.h>

/*
    fold.h — aggregate elements into a single value

    Derived utility.
    - Caller owns accumulator
    - No allocation
    - Order is left-to-right
*/

/* Fold function:
   acc = f(acc, item)
*/
typedef void (*FoldFn)(void *acc, void *item);

/*
    Folds items into accumulator.

    Requirements:
    - acc must point to valid, initialized storage
*/
static inline void fold(
    void *acc,
    void **items,
    size_t len,
    FoldFn f
) {
    if (!acc || !items || !f) return;

    for (size_t i = 0; i < len; i++) {
        f(acc, items[i]);
    }
}

#endif /* CANON_C_ALGO_FOLD_H */
