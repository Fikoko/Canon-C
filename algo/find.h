#ifndef CANON_C_ALGO_FIND_H
#define CANON_C_ALGO_FIND_H

#include <stddef.h>
#include <stdbool.h>

#include "data/vec.h"  // For vec integration macros

/*
    find.h — Locate first element matching predicate (functional style)

    algo_find:
      Scans left-to-right, returns true on first match.
      Optionally writes index and/or element pointer.

    Properties:
      - Read-only input
      - Short-circuiting
      - No allocation, mutation, or ownership
      - Optional user context
*/

typedef bool (*algo_find_pred)(const void* elem, void* ctx);

/* ============================================================
   Generic version (void* elements)
   ============================================================ */

/*
    algo_find:
      Finds first element where pred returns true.
      Returns true if found, false otherwise (or invalid input).
      Optionally stores index and/or element pointer.
*/
static inline bool algo_find(
    const void** items,
    size_t len,
    algo_find_pred pred,
    void* ctx,
    size_t* out_index,     // optional: receives matching index
    const void** out_elem  // optional: receives matching element pointer
)
{
    if (!items || !pred) return false;

    for (size_t i = 0; i < len; ++i) {
        if (pred(items[i], ctx)) {
            if (out_index) *out_index = i;
            if (out_elem)  *out_elem  = items[i];
            return true;
        }
    }
    return false;
}

/* ============================================================
   Strongly typed version (recommended)
   ============================================================ */

/*
    ALGO_FIND(array, len, Type, pred, ctx, out_index, out_elem)
      pred signature: bool pred(const Type* elem, void* ctx)
      out_index/out_elem: optional (pass NULL if not needed)
      Returns: true if found
*/
#define ALGO_FIND(array, len, Type, pred, ctx, out_index, out_elem) \
    ({ \
        bool _found = false; \
        if ((array) && (pred)) { \
            for (size_t _i = 0; _i < (len); ++_i) { \
                if ((pred)(&(array)[_i], (ctx))) { \
                    if (out_index) *(out_index) = _i; \
                    if (out_elem)  *(out_elem)  = &(array)[_i]; \
                    _found = true; \
                    break; \
                } \
            } \
        } \
        _found; \
    })

/* ============================================================
   Vec integration
   ============================================================ */

#define ALGO_FIND_VEC(vec, Type, pred, ctx, out_index, out_elem) \
    ALGO_FIND((vec).items, (vec).len, Type, pred, ctx, out_index, out_elem)

#endif /* CANON_C_ALGO_FIND_H */
