#ifndef CANON_C_DATA_STACK_H
#define CANON_C_DATA_STACK_H

#include "vec.h"

/*
    stack.h — LIFO stack (thin wrapper over vec)
*/

#define DEFINE_STACK(Type) \
typedef vec_##Type stack_##Type; \
\
static inline void stack_##Type##_init(stack_##Type* s, Type* buffer, size_t capacity) \
{ \
    vec_##Type##_init(s, buffer, capacity); \
} \
\
static inline bool stack_##Type##_push(stack_##Type* s, Type item) \
{ \
    result_bool_constcharp r = vec_##Type##_push(s, item); \
    return result_bool_constcharp_is_ok(r); \
} \
\
static inline bool stack_##Type##_pop(stack_##Type* s, Type* out) \
{ \
    return vec_##Type##_pop(s, out).is_ok; \
}

#endif /* CANON_C_DATA_STACK_H */
