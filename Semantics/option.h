#ifndef CANON_C_SEMANTICS_OPTION_H
#define CANON_C_SEMANTICS_OPTION_H

#include <stdbool.h>

// Macro to define Option type for any type
#define CANON_C_DEFINE_OPTION(type) \
typedef struct { \
    bool has_value; \
    type value; \
} Option_##type; \
\
static inline Option_##type Option_##type##_Some(type v) { \
    Option_##type o; o.has_value = true; o.value = v; return o; \
} \
\
static inline Option_##type Option_##type##_None() { \
    Option_##type o; o.has_value = false; return o; \
} \
\
static inline type Option_##type##_unwrap_or(Option_##type o, type fallback) { \
    return o.has_value ? o.value : fallback; \
}

#endif // CANON_C_SEMANTICS_OPTION_H
