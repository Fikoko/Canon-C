#ifndef CANON_C_ALGO_FILTER_H
#define CANON_C_ALGO_FILTER_H

#include <stddef.h>
#include <stdbool.h>
#include <canon/semantics/option.h>

/*
    filter.h — select elements based on predicate

    Semantics:
    - Preserves input order
    - Read-only over elements
    - No allocation
    - No ownership transfer

    Derived algorithm only.
*/

/* ============================================================
   Predicate
   ============================================================ */

/*
    Predicate function:
    - elem : element (read-only)
    - ctx  : optional user context
    - returns true to keep element
*/
typedef bool (*FilterPred)(const void *elem, void *ctx);

/* ============================================================
   Basic filter (caller-managed output)
   ============================================================ */

/*
    Filters items into caller-provided output buffer.

    Parameters:
    - items   : input array (read-only)
    - len     : number of elements
    - pred    : predicate function
    - ctx     : optional context
    - out     : output buffer (caller-owned)
    - out_cap : capacity of output buffer

    Returns:
    - number of elements written to `out`
    - truncates if out_cap < number of matches
*/
static inline size_t filter_into(
    const void **items,
    size_t       len,
    FilterPred   pred,
    void        *ctx,
    const void **out,
    size_t       out_cap
) {
    if (!items || !out || !pred)
        return 0;

    size_t out_len = 0;
    for (size_t i = 0; i < len && out_len < out_cap; i++) {
        if (pred(items[i], ctx)) {
            out[out_len++] = items[i];
        }
    }
    return out_len;
}

/* ============================================================
   Option<size_t> variant (explicit failure handling)
   ============================================================ */

CANON_C_DEFINE_OPTION(size_t)

/*
    Returns Some(number of elements written) or None if input/predicate/output is NULL
*/
static inline Option_size_t filter_into_opt(
    const void **items,
    size_t       len,
    FilterPred   pred,
    void        *ctx,
    const void **out,
    size_t       out_cap
) {
    if (!items || !out || !pred)
        return Option_size_t_None();

    size_t out_len = 0;
    for (size_t i = 0; i < len && out_len < out_cap; i++) {
        if (pred(items[i], ctx)) {
            out[out_len++] = items[i];
        }
    }
    return Option_size_t_Some(out_len);
}

#endif /* CANON_C_ALGO_FILTER_H */
