#ifndef CANON_C_SEMANTICS_OPTION_H
#define CANON_C_SEMANTICS_OPTION_H

#include <stdbool.h>
#include <assert.h>

/*
    option.h — Explicit presence or absence of a value

    Option<T> represents:
      - Some(value) : value is present
      - None        : value is absent

    No NULL pointers, no sentinel values, no hidden error states.
    Zero-cost abstraction: just a bool + the value (with natural alignment).
    Header-only, fully generic via macro.
*/

#define CANON_C_DEFINE_OPTION(type) \
typedef struct { \
    bool has_value; \
    type value; \
} option_##type; \
\
/* Construct Some(value) */ \
static inline option_##type option_##type##_some(type v) \
{ \
    option_##type o; \
    o.has_value = true; \
    o.value = v; \
    return o; \
} \
\
/* Construct None (zero-initialized → safe in static storage) */ \
static inline option_##type option_##type##_none(void) \
{ \
    option_##type o = {0}; \
    return o; \
} \
\
/* Presence checks */ \
static inline bool option_##type##_is_some(option_##type o) \
{ \
    return o.has_value; \
} \
\
static inline bool option_##type##_is_none(option_##type o) \
{ \
    return !o.has_value; \
} \
\
/* Safe extraction: writes to out pointer if present */ \
static inline bool option_##type##_get(option_##type o, type *out) \
{ \
    if (o.has_value && out) { \
        *out = o.value; \
        return true; \
    } \
    return false; \
} \
\
/* Return value or fallback */ \
static inline type option_##type##_unwrap_or(option_##type o, type fallback) \
{ \
    return o.has_value ? o.value : fallback; \
} \
\
/* Panic on None (debug only) */ \
static inline type option_##type##_unwrap(option_##type o) \
{ \
    assert(o.has_value && "option_##type##_unwrap called on None"); \
    return o.value; \
} \
\
/* Panic on None with custom message */ \
static inline type option_##type##_expect(option_##type o, const char *msg) \
{ \
    assert(o.has_value && msg); \
    (void)msg; /* silence unused warning in release */ \
    return o.value; \
} \
\
/* Transform value if present (map) */ \
static inline option_##type option_##type##_map( \
    option_##type o, \
    type (*f)(type) \
) { \
    return o.has_value ? option_##type##_some(f(o.value)) : option_##type##_none(); \
} \
\
/* Chain computations that return Option (flat_map / and_then) */ \
static inline option_##type option_##type##_and_then( \
    option_##type o, \
    option_##type (*f)(type) \
) { \
    return o.has_value ? f(o.value) : option_##type##_none(); \
} \
\
/* Provide alternative if None */ \
static inline option_##type option_##type##_or_else( \
    option_##type o, \
    option_##type (*fallback)(void) \
) { \
    return o.has_value ? o : fallback(); \
}

#endif /* CANON_C_SEMANTICS_OPTION_H */
