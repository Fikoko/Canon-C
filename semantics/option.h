#ifndef CANON_C_SEMANTICS_OPTION_H
#define CANON_C_SEMANTICS_OPTION_H

#include <stdbool.h>
#include <assert.h>

/*
    option.h — explicit presence / absence

    Option<T> encodes:
    - presence (Some)
    - absence (None)

    Without sentinels, magic values, or implicit error states.
    Header-only, zero-cost abstraction.
*/

#define CANON_C_DEFINE_OPTION(type)                                      \
typedef struct {                                                         \
    bool has_value;                                                      \
    type value;                                                          \
} Option_##type;                                                         \
                                                                         \
/* Construct Some(value) */                                              \
static inline Option_##type                                              \
Option_##type##_Some(type v)                                             \
{                                                                        \
    Option_##type o;                                                     \
    o.has_value = true;                                                  \
    o.value = v;                                                         \
    return o;                                                            \
}                                                                        \
                                                                         \
/* Construct None */                                                     \
static inline Option_##type                                              \
Option_##type##_None(void)                                               \
{                                                                        \
    Option_##type o = {0};                                               \
    return o;                                                            \
}                                                                        \
                                                                         \
/* Presence checks */                                                    \
static inline bool                                                       \
Option_##type##_is_some(Option_##type o)                                 \
{                                                                        \
    return o.has_value;                                                  \
}                                                                        \
                                                                         \
static inline bool                                                       \
Option_##type##_is_none(Option_##type o)                                 \
{                                                                        \
    return !o.has_value;                                                 \
}                                                                        \
                                                                         \
/* Safe access: writes value to out if present */                        \
static inline bool                                                       \
Option_##type##_get(                                                     \
    Option_##type o,                                                     \
    type *out                                                            \
)                                                                        \
{                                                                        \
    if (!o.has_value || !out)                                            \
        return false;                                                    \
                                                                         \
    *out = o.value;                                                      \
    return true;                                                         \
}                                                                        \
                                                                         \
/* Fallback extraction */                                                \
static inline type                                                       \
Option_##type##_unwrap_or(                                               \
    Option_##type o,                                                     \
    type fallback                                                        \
)                                                                        \
{                                                                        \
    return o.has_value ? o.value : fallback;                             \
}                                                                        \
                                                                         \
/* Asserted extraction (fail loudly if None) */                          \
static inline type                                                       \
Option_##type##_unwrap(Option_##type o) {                                \
    assert(o.has_value && "Called unwrap on None");                      \
    return o.value;                                                      \
}                                                                        \
                                                                         \
/* Asserted extraction with custom message */                             \
static inline type                                                       \
Option_##type##_expect(Option_##type o, const char *msg) {               \
    assert(o.has_value && msg);                                          \
    return o.value;                                                      \
}                                                                        \
                                                                         \
/* Functional combinators */                                              \
static inline Option_##type                                              \
Option_##type##_map(Option_##type o, type (*f)(type)) {                  \
    return o.has_value ? Option_##type##_Some(f(o.value)) : Option_##type##_None(); \
}
#endif /* CANON_C_SEMANTICS_OPTION_H */
