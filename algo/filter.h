#ifndef CANON_C_ALGO_FILTER_H
#define CANON_C_ALGO_FILTER_H

#include <stddef.h>
#include <stdbool.h>

#include "data/vec.h"  // For vec integration macros

/*
    filter.h — Select elements matching predicate (functional style)

    algo_filter_into:
      Copies elements that satisfy pred into caller-provided output buffer.
      Preserves relative order.
      Truncates if output capacity is insufficient.

    Properties:
      - Read-only input
      - No allocation, mutation, or ownership transfer
      - Short-circuits when output is full
      - Optional user context
*/

typedef bool (*algo_filter_pred)(const void* elem, void* ctx);

/* ============================================================
   Generic version (void* elements)
   ============================================================ */

/*
    algo_filter_into:
      Filters items into out buffer.
      Returns number of elements written (may be < matching count if truncated).
      Returns 0 on invalid input (items == NULL || out == NULL || pred == NULL).
*/
static inline size_t algo_filter_into(
    const void** items,
    size_t len,
    algo_filter_pred pred,
    void* ctx,
    void** out,          // Note: non-const — we write pointers
    size_t out_cap
)
{
    if (!items || !out || !pred) return 0;

    size_t out_len = 0;
    for (size_t i = 0; i < len; ++i) {
        if (out_len >= out_cap) break;  // truncate early
        if (pred(items[i], ctx)) {
            out[out_len++] = (void*)items[i];  // const cast: safe (borrowed pointer)
        }
    }
    return out_len;
}

/* ============================================================
   Strongly typed version (recommended)
   ============================================================ */

/*
    ALGO_FILTER_INTO(out_array, in_array, len, Type, pred, ctx)
      pred signature: bool pred(const Type* elem, void* ctx)
      Returns number of elements written
*/
#define ALGO_FILTER_INTO(out_array, in_array, len, Type, pred, ctx) \
    ({ \
        size_t _out_len = 0; \
        if ((out_array) && (in_array) && (pred)) { \
            const size_t _cap = (sizeof(out_array) / sizeof((out_array)[0])); \
            for (size_t _i = 0; _i < (len) && _out_len < _cap; ++_i) { \
                if ((pred)(&(in_array)[_i], (ctx))) { \
                    (out_array)[_out_len++] = (in_array)[_i]; \
                } \
            } \
        } \
        _out_len; \
    })

/* ============================================================
   Vec integration (safe bounded filtering)
   ============================================================ */

#define ALGO_FILTER_VEC(out_vec, in_vec, Type, pred, ctx) \
    ALGO_FILTER_INTO( \
        (out_vec).items, \
        (in_vec).items, \
        (in_vec).len, \
        Type, \
        pred, \
        ctx \
    )

#endif /* CANON_C_ALGO_FILTER_H */
