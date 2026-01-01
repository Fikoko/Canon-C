#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>
#include <canon/semantics/result.h>

/*
    map.h — element-wise transformation

    Semantics:
    - input[i] is transformed into output[i]
    - No allocation
    - No ownership transfer
    - No mutation of input elements
    - Output storage is caller-owned
*/

/* ------------------------------------------------------------
   Generic map function
   ------------------------------------------------------------ */

/*
    Map function (generic):
    - in  : pointer to input element (read-only)
    - out : pointer to output element (writable)
*/
typedef void (*MapFn)(void *out, const void *in);

/*
    Maps input items into output items.

    Parameters:
    - input  : array of pointers to input elements
    - output : array of writable pointers for output
    - len    : number of elements
    - f      : transformation function
*/
static inline void map(
    const void **input,
    void       **output,
    size_t       len,
    MapFn        f
) {
    if (!input || !output || !f)
        return;

    for (size_t i = 0; i < len; i++) {
        f(output[i], input[i]);
    }
}

/* ------------------------------------------------------------
   Optional typed array map (compile-time type safety)
   ------------------------------------------------------------ */

/*
    MAP_TYPED(out, in, len, Type, fn)

    - out  : pointer to output array
    - in   : pointer to input array
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

/* ------------------------------------------------------------
   Convenience macro for Vec_<type>
   ------------------------------------------------------------ */

/*
    MAP_VEC(out_vec, in_vec, Type, fn)

    - out_vec : destination Vec_<type>
    - in_vec  : source Vec_<type>
    - Type    : element type
    - fn      : function to apply
*/
#define MAP_VEC(out_vec, in_vec, Type, fn) \
    MAP_TYPED( \
        (out_vec).items, \
        (in_vec).items, \
        ((out_vec).len < (in_vec).len ? (out_vec).len : (in_vec).len), \
        Type, \
        fn \
    )

#endif /* CANON_C_ALGO_MAP_H */
