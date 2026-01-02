#ifndef CANON_C_ALGO_FOLD_H
#define CANON_C_ALGO_FOLD_H

#include <stddef.h>
#include <stdbool.h>

#include "data/vec.h"  // For vec integration

/*
    fold.h — Reduce sequence to single value (functional style)

    algo_fold:
      Applies folding function left-to-right to explicit accumulator.
      acc: caller-owned, caller-initialized, caller-mutable storage

    Properties:
      - No allocation, input mutation, or ownership transfer
      - Short-circuits on first error (fallible version)
      - Optional user context

    Perfect for sum, product, min/max, string concat, etc.
*/

typedef void (*algo_fold_fn)(void* acc, const void* item, void* ctx);

/* ============================================================
   Infallible fold (void callback)
   ============================================================ */

/*
    algo_fold:
      Folds items into caller-provided accumulator using f.
      No return value — acc is mutated in-place.
      Does nothing on invalid input.
*/
static inline void algo_fold(
    void* acc,           // caller-owned mutable accumulator
    const void** items,
    size_t len,
    algo_fold_fn f,
    void* ctx
)
{
    if (!acc || !items || !f) return;

    for (size_t i = 0; i < len; ++i) {
        f(acc, items[i], ctx);
    }
}

/* ============================================================
   Fallible fold (early termination on error)
   ============================================================ */

typedef result_bool_constcharp (*algo_fold_result_fn)(
    void* acc,
    const void* item,
    void* ctx
);

/*
    algo_fold_result:
      Fallible fold — stops and returns error on first failure.
      Uses shared result_bool_constcharp (true=continue, err=stop).
*/
static inline result_bool_constcharp algo_fold_result(
    void* acc,
    const void** items,
    size_t len,
    algo_fold_result_fn f,
    void* ctx
)
{
    if (!acc || !items || !f) {
        return result_bool_constcharp_err("invalid fold arguments");
    }

    for (size_t i = 0; i < len; ++i) {
        result_bool_constcharp r = f(acc, items[i], ctx);
        if (result_bool_constcharp_is_err(r)) {
            return r;  // early exit with error
        }
    }

    return result_bool_constcharp_ok(true);
}

/* ============================================================
   Strongly typed macros (recommended)
   ============================================================ */

/*
    ALGO_FOLD(acc_ptr, array, len, Type, fold_fn, ctx)
      fold_fn: void fold_fn(AccType* acc, const Type* item, void* ctx)
      acc_ptr: pointer to your accumulator variable
*/
#define ALGO_FOLD(acc_ptr, array, len, Type, fold_fn, ctx) \
    do { \
        if ((acc_ptr) && (array) && (fold_fn)) { \
            for (size_t _i = 0; _i < (len); ++_i) { \
                (fold_fn)((acc_ptr), &(array)[_i], (ctx)); \
            } \
        } \
    } while (0)

/*
    ALGO_FOLD_RESULT(acc_ptr, array, len, Type, fold_fn, ctx)
      fold_fn: result_bool_constcharp fold_fn(AccType* acc, const Type* item, void* ctx)
*/
#define ALGO_FOLD_RESULT(acc_ptr, array, len, Type, fold_fn, ctx) \
    ({ \
        result_bool_constcharp _r = result_bool_constcharp_ok(true); \
        if ((acc_ptr) && (array) && (fold_fn)) { \
            for (size_t _i = 0; _i < (len); ++_i) { \
                _r = (fold_fn)((acc_ptr), &(array)[_i], (ctx)); \
                if (result_bool_constcharp_is_err(_r)) break; \
            } \
        } else { \
            _r = result_bool_constcharp_err("invalid fold arguments"); \
        } \
        _r; \
    })

/* ============================================================
   Vec integration
   ============================================================ */

#define ALGO_FOLD_VEC(acc_ptr, vec, Type, fold_fn, ctx) \
    ALGO_FOLD((acc_ptr), (vec).items, (vec).len, Type, fold_fn, ctx)

#define ALGO_FOLD_RESULT_VEC(acc_ptr, vec, Type, fold_fn, ctx) \
    ALGO_FOLD_RESULT((acc_ptr), (vec).items, (vec).len, Type, fold_fn, ctx)

#endif /* CANON_C_ALGO_FOLD_H */
