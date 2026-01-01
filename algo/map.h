#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>

/*
    map.h — element-wise transformation

    Semantics:
    - input[i] is transformed into output[i]
    - No allocation
    - No ownership transfer
    - Output storage is caller-owned

    Derived algorithm only.
*/

/*
    Map function:
    - in  : pointer to input element
    - out : pointer to output element
*/
typedef void (*MapFn)(void *out, void *in);

/*
    Maps input items into output items.

    Requirements:
    - input  : array of pointers to elements
    - output : array of pointers to writable storage
    - Both arrays must be at least `len`
*/
static inline void map(
    void **input,
    void **output,
    size_t len,
    MapFn f
) {
    if (!input || !output || !f) return;

    for (size_t i = 0; i < len; i++) {
        f(output[i], input[i]);
    }
}

#endif /* CANON_C_ALGO_MAP_H */
