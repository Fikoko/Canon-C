#ifndef CANON_C_ALGO_ANY_ALL_H
#define CANON_C_ALGO_ANY_ALL_H

#include <stddef.h>
#include <stdbool.h>

#include "data/vec.h"  // For vec integration macros

/*
    any_all.h — Predicate checks over sequences (functional style)

    algo_any: true if at least one element satisfies predicate
    algo_all: true if all elements satisfy predicate

    Properties:
      - Read-only access
      - Short-circuiting
      - No allocation, mutation, or ownership
      - Optional user context
*/

typedef bool (*algo_pred_fn)(const void* elem, void* ctx);

/* ============================================================
   Generic versions (void* elements)
   ============================================================ */

/*
    algo_any:
      Returns true if pred returns true for any element.
      Returns false if items == NULL or pred == NULL or len == 0.
*/
static inline bool algo_any(
    const void** items,
    size_t len,
    algo_pred_fn pred,
    void* ctx
)
{
    if (!items || !pred) return false;

    for (size_t i = 0; i < len; ++i) {
        if (pred(items[i], ctx)) {
            return true;
        }
    }
    return false;
}

/*
    algo_all:
      Returns true if pred returns true for every element.
      Returns false if any element fails or on invalid input.
*/
static inline bool algo_all(
    const void** items,
    size_t len,
    algo_pred_fn pred,
    void* ctx
)
{
    if (!items || !pred) return false;

    for (size_t i = 0; i < len; ++i) {
        if (!pred(items[i], ctx)) {
            return false;
        }
    }
    return true;
}

/* ============================================================
   Strongly typed versions (recommended)
   ============================================================ */

#define ALGO_ANY_TYPED(items, len, Type, pred, ctx) \
    ({ \
        bool _result = false; \
        if ((items) && (pred)) { \
            for (size_t _i = 0; _i < (len); ++_i) { \
                if ((pred)(&(items)[_i], (ctx))) { \
                    _result = true; \
                    break; \
                } \
            } \
        } \
        _result; \
    })

#define ALGO_ALL_TYPED(items, len, Type, pred, ctx) \
    ({ \
        bool _result = true; \
        if ((items) && (pred)) { \
            for (size_t _i = 0; _i < (len); ++_i) { \
                if (!(pred)(&(items)[_i], (ctx))) { \
                    _result = false; \
                    break; \
                } \
            } \
        } else { \
            _result = false; \
        } \
        _result; \
    })

/* ============================================================
   Vec integration (safe and convenient)
   ============================================================ */

#define ALGO_ANY_VEC(vec, Type, pred, ctx) \
    ALGO_ANY_TYPED((vec).items, (vec).len, Type, pred, ctx)

#define ALGO_ALL_VEC(vec, Type, pred, ctx) \
    ALGO_ALL_TYPED((vec).items, (vec).len, Type, pred, ctx)

#endif /* CANON_C_ALGO_ANY_ALL_H */
