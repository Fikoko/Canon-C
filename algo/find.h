#ifndef CANON_C_ALGO_FIND_H
#define CANON_C_ALGO_FIND_H

#include <stddef.h>
#include <stdbool.h>

/*
    find.h — locate first element matching a predicate

    Semantics:
    - Scans a sequence left-to-right
    - Read-only over elements
    - No allocation
    - No ownership transfer

    Result is reported explicitly via out-parameters.
*/

/*
    Predicate function:
    - elem : current element (read-only)
    - ctx  : optional user context (may be NULL)
*/
typedef bool (*FindPred)(const void *elem, void *ctx);

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

#endif /* CANON_C_ALGO_FIND_H */
