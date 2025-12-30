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
static inline value_type                                                  \
Result_##value_type##_##error_type##_unwrap_or(                          \
    Result_##value_type##_##error_type r,                                 \
    value_type fallback                                                   \
) {                                                                      \
    return r.is_ok ? r.ok : fallback;                                    \
}

#endif /* CANON_C_SEMANTICS_RESULT_H */
