#ifndef CANON_C_ALGO_FIND_H
#define CANON_C_ALGO_FIND_H

#include <stddef.h>
#include <stdbool.h>
#include <canon/semantics/option.h>

/*
    find.h — locate first element matching a predicate

    Semantics:
    - Scans a sequence left-to-right
    - Read-only over elements
    - No allocation
    - No ownership transfer

    Provides explicit and functional-style search results.
*/

/* ============================================================
   Predicate function
   ============================================================ */

/*
    Predicate function:
    - elem : current element (read-only)
    - ctx  : optional user context (may be NULL)
*/
typedef bool (*FindPred)(const void *elem, void *ctx);

/* ============================================================
   Basic find (out-parameter style)
   ============================================================ */

/*
    Finds the first element matching `pred`.

    Parameters:
    - items     : array of element pointers (read-only)
    - len       : number of elements
    - pred      : predicate function
    - ctx       : optional context
    - out_index : optional; receives index if found
    - out_elem  : optional; receives element pointer if found

    Returns:
    - true  if a matching element was found
    - false otherwise
*/
static inline bool find(
    const void **items,
    size_t       len,
    FindPred     pred,
    void        *ctx,
    size_t      *out_index,
    const void **out_elem
) {
    if (!items || !pred)
        return false;

    for (size_t i = 0; i < len; i++) {
        if (pred(items[i], ctx)) {
            if (out_index) *out_index = i;
            if (out_elem)  *out_elem  = items[i];
            return true;
        }
    }

    return false;
}

/* ============================================================
   Option-based variants (functional, explicit)
   ============================================================ */

CANON_C_DEFINE_OPTION(size_t)
CANON_C_DEFINE_OPTION(void*)

/*
    Returns Some(index) if element matches predicate, None if not found
*/
static inline Option_size_t find_index_opt(
    const void **items,
    size_t       len,
    FindPred     pred,
    void        *ctx
) {
    if (!items || !pred)
        return Option_size_t_None();

    for (size_t i = 0; i < len; i++) {
        if (pred(items[i], ctx))
            return Option_size_t_Some(i);
    }

    return Option_size_t_None();
}

/*
    Returns Some(element pointer) if match found, None otherwise
*/
static inline Option_void_ptr find_elem_opt(
    const void **items,
    size_t       len,
    FindPred     pred,
    void        *ctx
) {
    if (!items || !pred)
        return Option_void_ptr_None();

    for (size_t i = 0; i < len; i++) {
        if (pred(items[i], ctx))
            return Option_void_ptr_Some((void*)items[i]);
    }

    return Option_void_ptr_None();
}

#endif /* CANON_C_ALGO_FIND_H */
