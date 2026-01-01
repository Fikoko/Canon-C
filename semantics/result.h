#ifndef CANON_C_SEMANTICS_RESULT_H
#define CANON_C_SEMANTICS_RESULT_H

#include <stdbool.h>

/*
    result.h — explicit success / failure

    Result<T, E> represents:
    - success with value T
    - failure with error E

    No implicit flags.
    No sentinel values.
    No exceptions.
*/

#define CANON_C_DEFINE_RESULT(value_type, error_type)                    \
typedef struct {                                                         \
    bool is_ok;                                                          \
    union {                                                              \
        value_type ok;                                                   \
        error_type err;                                                  \
    };                                                                   \
} Result_##value_type##_##error_type;                                    \
                                                                         \
/* Constructors */                                                       \
static inline Result_##value_type##_##error_type                          \
Result_##value_type##_##error_type##_Ok(value_type v) {                  \
    Result_##value_type##_##error_type r = {0};                          \
    r.is_ok = true;                                                      \
    r.ok = v;                                                            \
    return r;                                                            \
}                                                                        \
                                                                         \
static inline Result_##value_type##_##error_type                          \
Result_##value_type##_##error_type##_Err(error_type e) {                 \
    Result_##value_type##_##error_type r = {0};                          \
    r.is_ok = false;                                                     \
    r.err = e;                                                           \
    return r;                                                            \
}                                                                        \
                                                                         \
/* State checks */                                                        \
static inline bool                                                       \
Result_##value_type##_##error_type##_is_ok(                              \
    Result_##value_type##_##error_type r                                 \
) {                                                                      \
    return r.is_ok;                                                      \
}                                                                        \
                                                                         \
static inline bool                                                       \
Result_##value_type##_##error_type##_is_err(                             \
    Result_##value_type##_##error_type r                                 \
) {                                                                      \
    return !r.is_ok;                                                     \
}                                                                        \
                                                                         \
/* Safe extraction */                                                    \
static inline bool                                                       \
Result_##value_type##_##error_type##_get_ok(                             \
    Result_##value_type##_##error_type r,                                \
    value_type *out                                                      \
) {                                                                      \
    if (!r.is_ok || !out)                                                \
        return false;                                                    \
                                                                         \
    *out = r.ok;                                                         \
    return true;                                                         \
}                                                                        \
                                                                         \
static inline bool                                                       \
Result_##value_type##_##error_type##_get_err(                            \
    Result_##value_type##_##error_type r,                                \
    error_type *out                                                      \
) {                                                                      \
    if (r.is_ok || !out)                                                 \
        return false;                                                    \
                                                                         \
    *out = r.err;                                                        \
    return true;                                                         \
}                                                                        \
                                                                         \
/* Fallback extraction */                                                \
static inline value_type                                                 \
Result_##value_type##_##error_type##_unwrap_or(                          \
    Result_##value_type##_##error_type r,                                \
    value_type fallback                                                  \
) {                                                                      \
    return r.is_ok ? r.ok : fallback;                                    \
}

#endif /* CANON_C_SEMANTICS_RESULT_H */
