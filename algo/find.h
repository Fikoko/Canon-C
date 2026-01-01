#ifndef CANON_C_ALGO_FIND_H
#define CANON_C_ALGO_FIND_H

#include <stddef.h>
#include <stdbool.h>

/*
    find.h — locate first element matching a predicate

    Semantics:
    - Scans a sequence left-to-right
    - Returns whether a match was found
    - Does NOT allocate
    - Does NOT own memory
    - Does NOT return newly created values

    The result is communicated explicitly via out-parameters.
*/

/*
    Predicate function:
    - elem : pointer to element
    - ctx  : optional user context (may be NULL)
*/
typedef bool (*FindPred)(void *elem, void *ctx);

/*
    Finds the first element matching `pred`.

    Parameters:
    - items     : array of element pointers
    - len       : number of elements
    - pred      : predicate function
    - ctx       : optional context passed to predicate
    - out_index : optional; receives index if found
    - out_elem  : optional; receives element pointer if found

    Returns:
    - true  if a matching element was found
    - false otherwise
*/
static inline bool find(
    void      **items,
    size_t      len,
    FindPred    pred,
    void       *ctx,
    size_t     *out_index,
    void      **out_elem
) {
    if (!items || !pred) return false;

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

