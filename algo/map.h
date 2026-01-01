#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>

/*
    map.h — element-wise transformation

    Semantics:
    - input[i] is transformed into output[i]
    - No allocation
    - No ownership transfer
    - No mutation of input elements
    - Output storage is caller-owned

    Derived algorithm only.
*/

/*
    Map function (generic):
    - in  : pointer to input element
    - out : pointer to output element (must be writable)
*/
typedef void (*MapFn)(void *out, const void *in);

/*
    Maps input items into output items.
    Requirements:
    - input  : array of pointers to input elements
    - output : array of pointers to writable output storage
    - Both arrays must be at least `len`
*/
static inline void map(
    void **input,
    void **output,
    size_t len,
    MapFn f
) {
    if (!input || !output || !f)
        return;

    for (size_t i = 0; i < len; i++) {
        f(output[i], input[i]);
    }
}

/* ============================================================
   Typed macro (for arrays or Vec_<type>)
   ============================================================ */

/*
    MAP_TYPED(out, in, len, Type, fn)

    - out  : pointer to output array or Vec_<type>.items
    - in   : pointer to input array or Vec_<type>.items
    - len  : number of elements
    - Type : element type
    - fn   : function of signature `void fn(Type *out, const Type *in)`
*/
#define MAP_TYPED(out, in, len, Type, fn) \
    do { \
        if ((out) && (in) && (fn)) { \
            for (size_t i = 0; i < (len); i++) { \
                fn(&(out)[i], &(in)[i]); \
            } \
        } \
    } while(0)

/*
    MAP_VEC(out_vec, in_vec, Type, fn)

    Convenience macro for Vec_<type>:

    - out_vec : destination Vec_<type>
    - in_vec  : source Vec_<type>
    - Type    : element type
    - fn      : transformation function
*/
#define MAP_VEC(out_vec, in_vec, Type, fn) \
    MAP_TYPED((out_vec).items, (in_vec).items, ((out_vec).len < (in_vec).len ? (out_vec).len : (in_vec).len), Type, fn)

#endif /* CANON_C_ALGO_MAP_H */
