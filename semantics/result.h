#ifndef CANON_C_SEMANTICS_RESULT_H
#define CANON_C_SEMANTICS_RESULT_H

#include <stdbool.h>
#include <assert.h>

/*
    result.h — Explicit success or failure with value or error

    Result<T, E> represents:
      - Ok(value)  : successful computation with value of type T
      - Err(error) : failed computation with error of type E

    No exceptions, no errno, no sentinel values.
    Fully explicit, zero-cost abstraction.
    Header-only, generic via macro.
*/

#define CANON_C_DEFINE_RESULT(value_type, error_type) \
typedef struct { \
    bool is_ok; \
    union { \
        value_type ok; \
        error_type err; \
    }; \
} result_##value_type##_##error_type; \
\
/* Construct Ok(value) */ \
static inline result_##value_type##_##error_type \
result_##value_type##_##error_type##_ok(value_type v) \
{ \
    result_##value_type##_##error_type r = {0}; \
    r.is_ok = true; \
    r.ok = v; \
    return r; \
} \
\
/* Construct Err(error) */ \
static inline result_##value_type##_##error_type \
result_##value_type##_##error_type##_err(error_type e) \
{ \
    result_##value_type##_##error_type r = {0}; \
    r.is_ok = false; \
    r.err = e; \
    return r; \
} \
\
/* State checks */ \
static inline bool \
result_##value_type##_##error_type##_is_ok(result_##value_type##_##error_type r) \
{ \
    return r.is_ok; \
} \
\
static inline bool \
result_##value_type##_##error_type##_is_err(result_##value_type##_##error_type r) \
{ \
    return !r.is_ok; \
} \
\
/* Safe extraction */ \
static inline bool \
result_##value_type##_##error_type##_get_ok( \
    result_##value_type##_##error_type r, \
    value_type *out \
) { \
    if (r.is_ok && out) { \
        *out = r.ok; \
        return true; \
    } \
    return false; \
} \
\
static inline bool \
result_##value_type##_##error_type##_get_err( \
    result_##value_type##_##error_type r, \
    error_type *out \
) { \
    if (!r.is_ok && out) { \
        *out = r.err; \
        return true; \
    } \
    return false; \
} \
\
/* Fallback extraction */ \
static inline value_type \
result_##value_type##_##error_type##_unwrap_or( \
    result_##value_type##_##error_type r, \
    value_type fallback \
) { \
    return r.is_ok ? r.ok : fallback; \
} \
\
/* Panic on Err (debug only) */ \
static inline value_type \
result_##value_type##_##error_type##_unwrap(result_##value_type##_##error_type r) \
{ \
    assert(r.is_ok && "result_##value_type##_##error_type##_unwrap called on Err"); \
    return r.ok; \
} \
\
static inline value_type \
result_##value_type##_##error_type##_expect( \
    result_##value_type##_##error_type r, \
    const char *msg \
) { \
    assert(r.is_ok && msg); \
    (void)msg; \
    return r.ok; \
} \
\
/* Transform success value (map) */ \
static inline result_##value_type##_##error_type \
result_##value_type##_##error_type##_map( \
    result_##value_type##_##error_type r, \
    value_type (*f)(value_type) \
) { \
    return r.is_ok ? result_##value_type##_##error_type##_ok(f(r.ok)) : r; \
} \
\
/* Transform error value (map_err) */ \
static inline result_##value_type##_##error_type \
result_##value_type##_##error_type##_map_err( \
    result_##value_type##_##error_type r, \
    error_type (*f)(error_type) \
) { \
    return r.is_ok ? r : result_##value_type##_##error_type##_err(f(r.err)); \
} \
\
/* Chain computation that returns Result (and_then / flat_map) */ \
static inline result_##value_type##_##error_type \
result_##value_type##_##error_type##_and_then( \
    result_##value_type##_##error_type r, \
    result_##value_type##_##error_type (*f)(value_type) \
) { \
    return r.is_ok ? f(r.ok) : r; \
} \
\
/* Provide alternative Result if Err */ \
static inline result_##value_type##_##error_type \
result_##value_type##_##error_type##_or_else( \
    result_##value_type##_##error_type r, \
    result_##value_type##_##error_type (*fallback)(error_type) \
) { \
    return r.is_ok ? r : fallback(r.err); \
}

#endif /* CANON_C_SEMANTICS_RESULT_H */
