#ifndef CANON_C_ALGO_ANY_ALL_H
#define CANON_C_ALGO_ANY_ALL_H

#include <stddef.h>
#include <stdbool.h>

/*
    any_all.h — predicate checks over sequences

    Semantics:
    - any: returns true if ANY element satisfies predicate
    - all: returns true if ALL elements satisfy predicate

    Properties:
    - No allocation
    - No ownership
    - No mutation
    - Short-circuiting behavior
*/

/*
    Predicate function:
    - elem : pointer to element
    - ctx  : optional user context (may be NULL)
*/
typedef bool (*AnyAllPred)(void *elem, void *ctx);

/* ============================================================
   any
   ============================================================ */

/*
    Returns true if predicate is true for at least one element.
*/
static inline bool any(
    void        **items,
    size_t        len,
    AnyAllPred    pred,
    void         *ctx
) {
    if (!items || !pred) return false;

    for (size_t i = 0; i < len; i++) {
        if (pred(items[i], ctx))
            return true;
    }

    return false;
}

/* ============================================================
   all
   ============================================================ */

/*
    Returns true if predicate is true for all elements.
*/
static inline bool all(
    void        **items,
    size_t        len,
    AnyAllPred    pred,
    void         *ctx
) {
    if (!items || !pred) return false;

    for (size_t i = 0; i < len; i++) {
        if (!pred(items[i], ctx))
            return false;
    }

    return true;
}

#endif /* CANON_C_ALGO_ANY_ALL_H */

