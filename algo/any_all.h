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
    - Read-only
    - No allocation
    - No ownership
    - No mutation
    - Short-circuiting
*/

/*
    Predicate function:
    - elem : element (read-only)
    - ctx  : optional user context (may be NULL)
*/
typedef bool (*AnyAllPred)(const void *elem, void *ctx);

/* ============================================================
   any
   ============================================================ */

/*
    Returns true if predicate is true for at least one element.

    For empty sequences:
    - returns false
*/
static inline bool any(
    const void **items,
    size_t       len,
    AnyAllPred   pred,
    void        *ctx
) {
    if (!items || !pred)
        return false;

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

    For empty sequences:
    - returns true
*/
static inline bool all(
    const void **items,
    size_t       len,
    AnyAllPred   pred,
    void        *ctx
) {
    if (!items || !pred)
        return false;

    for (size_t i = 0; i < len; i++) {
        if (!pred(items[i], ctx))
            return false;
    }

    return true;
}

#endif /* CANON_C_ALGO_ANY_ALL_H */
