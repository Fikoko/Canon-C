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

/*
    Integer range [start, end) with step
*/
typedef struct {
    size_t current;
    size_t end;
    size_t step;
} Range;

/* ============================================================
   Construction
   ============================================================ */

/*
    Create range: [start, end) with step

    Notes:
    - step == 0 is normalized to 1
    - If start >= end, range is empty
*/
static inline Range range_make(size_t start, size_t end, size_t step) {
    Range r;
    r.current = start;
    r.end     = end;
    r.step    = step ? step : 1;
    return r;
}

/*
    Shorthand: range [0, end)
*/
static inline Range range_upto(size_t end) {
    return range_make(0, end, 1);
}

/* ============================================================
   Iteration
   ============================================================ */

/*
    Returns true if another value can be produced
*/
static inline bool range_has_next(const Range *r) {
    return r && r->current < r->end;
}

/*
    Produces next value and advances the range.

    Requirements:
    - range_has_next(r) must be true

    Behavior:
    - Never overflows
    - Stops cleanly at end
*/
static inline size_t range_next(Range *r) {
    size_t value = r->current;

    if (r->end - r->current <= r->step)
        r->current = r->end;
    else
        r->current += r->step;

    return value;
}

#endif /* CANON_C_DATA_RANGE_H */
