#ifndef CANON_C_DATA_RANGE_H
#define CANON_C_DATA_RANGE_H

#include <stddef.h>
#include <stdbool.h>

/*
    range.h — sequential integer generator

    Derived data utility.
    - No allocation
    - No ownership
    - No hidden state
    - Explicit iteration
*/

/* Integer range [start, end) with step */
typedef struct {
    size_t current;
    size_t end;
    size_t step;
} Range;

/* ------------------------------------------------------------
   Construction
   ------------------------------------------------------------ */

/* Create range: [start, end) with step */
static inline Range range_make(size_t start, size_t end, size_t step) {
    Range r;
    r.current = start;
    r.end = end;
    r.step = step ? step : 1;
    return r;
}

/* Shorthand: range from 0 to end */
static inline Range range_upto(size_t end) {
    return range_make(0, end, 1);
}

/* ------------------------------------------------------------
   Iteration
   ------------------------------------------------------------ */

/* Check if more values remain */
static inline bool range_has_next(const Range *r) {
    return r && r->current < r->end;
}

/* Get next value and advance */
static inline size_t range_next(Range *r) {
    size_t value = r->current;
    r->current += r->step;
    return value;
}

#endif /* CANON_C_DATA_RANGE_H */

