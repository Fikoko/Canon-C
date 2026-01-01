#ifndef CANON_C_ALGO_FILTER_H
#define CANON_C_ALGO_FILTER_H

#include <stddef.h>
#include <stdbool.h>
#include <canon/core/memory.h>

/*
    filter.h — select elements based on predicate

    Semantics:
    - Preserves input order
    - No ownership transfer
    - No hidden allocation
*/

/*
    Predicate:
    - item : element
    - ctx  : user context
    - return true to keep element
*/
typedef bool (*FilterPred)(void *item, void *ctx);

/* ============================================================
   Manual buffer mode
   ============================================================ */

/*
    Filters items into caller-provided output buffer.

    Returns number of elements written.
    Stops early if output buffer is full.
*/
static inline size_t filter_into(
    void      **items,
    size_t      len,
    FilterPred  pred,
    void       *ctx,
    void      **out,
    size_t      out_cap
) {
    if (!items || !out || !pred) return 0;

    size_t out_len = 0;

    for (size_t i = 0; i < len && out_len < out_cap; i++) {
        if (pred(items[i], ctx)) {
            out[out_len++] = items[i];
        }
    }

    return out_len;
}

/* ============================================================
   Explicit allocation mode
   ============================================================ */

/*
    Allocates output array sized to input length.
    Returns number of filtered elements.
*/
static inline bool filter_alloc(
    void      **items,
    size_t      len,
    FilterPred  pred,
    void       *ctx,
    void     ***out_items,
    size_t     *out_len
) {
    if (!items || !pred || !out_items || !out_len) return false;

    void **out = (void **)mem_alloc(sizeof(void *) * len);
    if (!out) return false;

    size_t count = 0;

    for (size_t i = 0; i < len; i++) {
        if (pred(items[i], ctx)) {
            out[count++] = items[i];
        }
    }

    *out_items = out;
    *out_len   = count;
    return true;
}

#endif /* CANON_C_ALGO_FILTER_H */
