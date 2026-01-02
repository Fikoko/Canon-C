#ifndef CANON_C_DATA_RANGE_H
#define CANON_C_DATA_RANGE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>  // for SIZE_MAX
#include <limits.h>  // for INT_MAX/MIN if signed

/*
    range.h — Explicit integer range generator

    Generates sequential integers with explicit iteration.
    Supports:
      - Unsigned and signed integers
      - Ascending and descending (positive/negative step)
      - Empty ranges
      - No allocation, no hidden state, no ownership

    Perfect for bounded loops without index bugs.
*/

typedef struct {
    ptrdiff_t current;  // signed for flexibility (supports negative values and descending)
    ptrdiff_t end;      // exclusive upper bound
    ptrdiff_t step;     // non-zero step (positive or negative)
} range;

/* ============================================================
   Construction
   ============================================================ */

/*
    range_make(start, end, step):
      Creates range [start, end) with given step.
      - step == 0 → normalized to +1
      - If step > 0 and start >= end → empty
      - If step < 0 and start <= end → empty
*/
static inline range range_make(ptrdiff_t start, ptrdiff_t end, ptrdiff_t step)
{
    if (step == 0) step = 1;

    return (range){
        .current = start,
        .end     = end,
        .step    = step
    };
}

/* Common shorthands */
static inline range range_upto(ptrdiff_t end)          // [0, end)
{
    return range_make(0, end, 1);
}

static inline range range_from_to(ptrdiff_t start, ptrdiff_t end)  // [start, end)
{
    return range_make(start, end, 1);
}

static inline range range_downfrom(ptrdiff_t start)   // [start, 0) descending
{
    return range_make(start, 0, -1);
}

/* ============================================================
   Queries
   ============================================================ */

static inline bool range_is_empty(const range* r)
{
    if (!r) return true;
    if (r->step > 0) return r->current >= r->end;
    if (r->step < 0) return r->current <= r->end;
    return true;  // step == 0 should not happen
}

static inline bool range_has_next(const range* r)
{
    return !range_is_empty(r);
}

/* Estimated length (safe, no overflow) */
static inline size_t range_len(const range* r)
{
    if (!r || range_is_empty(r)) return 0;

    if (r->step > 0) {
        if (r->end - r->current > (ptrdiff_t)SIZE_MAX) return SIZE_MAX;
        return (size_t)((r->end - r->current - 1) / r->step + 1);
    } else {
        if (r->current - r->end > (ptrdiff_t)SIZE_MAX) return SIZE_MAX;
        return (size_t)((r->current - r->end - 1) / -r->step + 1);
    }
}

/* ============================================================
   Iteration
   ============================================================ */

/*
    range_next(r):
      Returns next value and advances the iterator.
      Behavior is defined only if range_has_next(r) was true.
      On exhaustion: returns last value and sets current to end
*/
static inline ptrdiff_t range_next(range* r)
{
    if (!r || range_is_empty(r)) return 0;  // safety

    ptrdiff_t value = r->current;

    if (r->step > 0) {
        if (r->current > r->end - r->step) {
            r->current = r->end;  // exhaust
        } else {
            r->current += r->step;
        }
    } else {
        if (r->current < r->end - r->step) {
            r->current = r->end;
        } else {
            r->current += r->step;
        }
    }

    return value;
}

/* Reset to beginning */
static inline void range_reset(range* r, ptrdiff_t start)
{
    if (r) r->current = start;
}

/* ============================================================
   Convenience: for-loop integration
   ============================================================ */

#define RANGE_FOR(var, r_expr) \
    for (range _r = (r_expr), *_rp = &_r; \
         range_has_next(_rp) && ((var) = range_next(_rp), 1); )

#endif /* CANON_C_DATA_RANGE_H */
