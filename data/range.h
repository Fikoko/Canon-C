#ifndef CANON_C_DATA_RANGE_H
#define CANON_C_DATA_RANGE_H

#include <stddef.h>
#include <stdbool.h>

/*
    range.h — sequential integer generator

    Semantics:
    - Generates integers in [start, end)
    - Step is positive and non-zero
    - Explicit iteration
    - No allocation
    - No ownership
    - No hidden state
*/

/* ============================================================
   Range type
   ============================================================ */

typedef struct {
    size_t current;  /* next value to produce */
    size_t end;      /* exclusive end */
    size_t step;     /* iteration step (>0) */
} Range;

/* ============================================================
   Construction
   ============================================================ */

/*
    Create a range [start, end) with step.
    - step == 0 → normalized to 1
    - If start >= end → empty range
*/
static inline Range range_make(size_t start, size_t end, size_t step) {
    Range r;
    r.current = start;
    r.end     = end;
    r.step    = step ? step : 1;
    return r;
}

/* Shorthand: range [0, end) with step 1 */
static inline Range range_upto(size_t end) {
    return range_make(0, end, 1);
}

/* ============================================================
   Iteration
   ============================================================ */

/* Returns true if another value can be produced */
static inline bool range_has_next(const Range *r) {
    return r && r->current < r->end;
}

/* Produces next value and advances the range.
   Requirements:
   - range_has_next(r) must be true
   - step > 0
*/
static inline size_t range_next(Range *r) {
    if (!r) return 0;  /* safety */

    size_t value = r->current;

    /* advance safely without overflow */
    if (r->end - r->current <= r->step)
        r->current = r->end;
    else
        r->current += r->step;

    return value;
}

/* Reset range to start */
static inline void range_reset(Range *r, size_t start) {
    if (r) r->current = start;
}

#endif /* CANON_C_DATA_RANGE_H */
