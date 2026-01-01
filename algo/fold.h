#ifndef CANON_C_ALGO_FOLD_H
#define CANON_C_ALGO_FOLD_H

#include <stddef.h>
#include <canon/semantics/result.h>

/*
    fold.h — aggregate elements into a single value

    Semantics:
    - Left-to-right fold
    - Explicit accumulator
    - No allocation
    - No ownership transfer
    - Does not mutate input elements
*/

/* ============================================================
   Fold function
   ============================================================ */

/*
    Fold function:
    - acc  : accumulator (caller-owned, mutable)
    - item : current element (read-only)
    - ctx  : optional user context (may be NULL)
*/
typedef void (*FoldFn)(void *acc, const void *item, void *ctx);

/* ============================================================
   Basic fold (read-only items)
   ============================================================ */

/*
    Fold items into accumulator.

    Requirements:
    - acc must point to valid, initialized storage
    - items must contain at least `len` elements
*/
static inline void fold(
    void           *acc,
    const void    **items,
    size_t          len,
    FoldFn          f,
    void           *ctx
) {
    if (!acc || !items || !f)
        return;

    for (size_t i = 0; i < len; i++) {
        f(acc, items[i], ctx);
    }
}

/* ============================================================
   Result-based fold (optional, handles possible failure)
   ============================================================ */

CANON_C_DEFINE_RESULT(void*, const char*)

/*
    Fold with a function that may fail.

    Requirements:
    - f must return Result<void*, const char*>
    - Iteration stops immediately on first Err
*/
typedef Result_void_ptr_const_char_ptr (*FoldFnResult)(void *acc, const void *item, void *ctx);

static inline Result_void_ptr_const_char_ptr fold_result(
    void            *acc,
    const void     **items,
    size_t           len,
    FoldFnResult     f,
    void            *ctx
) {
    if (!acc || !items || !f)
        return Result_void_ptr_const_char_ptr_Err("Invalid arguments");

    for (size_t i = 0; i < len; i++) {
        Result_void_ptr_const_char_ptr r = f(acc, items[i], ctx);
        if (r.is_err)
            return r;
    }

    return Result_void_ptr_const_char_ptr_Ok(NULL);
}

#endif /* CANON_C_ALGO_FOLD_H */
