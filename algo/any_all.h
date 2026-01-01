#ifndef CANON_C_ALGO_ANY_ALL_H
#define CANON_C_ALGO_ANY_ALL_H

#include <stddef.h>
#include <stdbool.h>
#include <canon/semantics/option.h>

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

/* ============================================================
   Predicate function type
   ============================================================ */

/*
    Predicate function:
    - elem : element (read-only)
    - ctx  : optional user context (may be NULL)
*/
typedef bool (*AnyAllPred)(const void *elem, void *ctx);

/* ============================================================
   Basic versions (return raw bool)
   ============================================================ */

/* Returns true if predicate is true for at least one element */
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

/* Returns true if predicate is true for all elements */
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

/* ============================================================
   Option<bool> variants (explicit failure handling)
   ============================================================ */

CANON_C_DEFINE_OPTION(bool)

/* Returns Some(true/false) or None if input/predicate is NULL */
static inline Option_bool any_opt(
    const void **items,
    size_t       len,
    AnyAllPred   pred,
    void        *ctx
) {
    if (!items || !pred) return Option_bool_None();

    for (size_t i = 0; i < len; i++) {
        if (pred(items[i], ctx))
            return Option_bool_Some(true);
    }
    return Option_bool_Some(false);
}

/* Returns Some(true/false) or None if input/predicate is NULL */
static inline Option_bool all_opt(
    const void **items,
    size_t       len,
    AnyAllPred   pred,
    void        *ctx
) {
    if (!items || !pred) return Option_bool_None();

    for (size_t i = 0; i < len; i++) {
        if (!pred(items[i], ctx))
            return Option_bool_Some(false);
    }
    return Option_bool_Some(true);
}

#endif /* CANON_C_ALGO_ANY_ALL_H */
