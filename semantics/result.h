#ifndef CANON_C_SEMANTICS_RESULT_H
#define CANON_C_SEMANTICS_RESULT_H

#include <stdbool.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
    result.h — explicit success / failure with combinators

    Result<T, E> represents:
    - success with value T
    - failure with error E

    No implicit flags.
    No sentinel values.
    No exceptions.
    Fully explicit memory and failure semantics.
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
/* State checks */                                                       \
static inline bool                                                       \
Result_##value_type##_##error_type##_is_ok(Result_##value_type##_##error_type r) { \
    return r.is_ok;                                                      \
}                                                                        \
                                                                         \
static inline bool                                                       \
Result_##value_type##_##error_type##_is_err(Result_##value_type##_##error_type r) { \
    return !r.is_ok;                                                     \
}                                                                        \
                                                                         \
/* Safe extraction */                                                     \
static inline bool                                                       \
Result_##value_type##_##error_type##_get_ok(Result_##value_type##_##error_type r, value_type *out) { \
    if (!r.is_ok || !out) return false;                                  \
    *out = r.ok;                                                         \
    return true;                                                         \
}                                                                        \
                                                                         \
static inline bool                                                       \
Result_##value_type##_##error_type##_get_err(Result_##value_type##_##error_type r, error_type *out) { \
    if (r.is_ok || !out) return false;                                   \
    *out = r.err;                                                        \
    return true;                                                         \
}                                                                        \
                                                                         \
/* Fallback extraction */                                                 \
static inline value_type                                                 \
Result_##value_type##_##error_type##_unwrap_or(Result_##value_type##_##error_type r, value_type fallback) { \
    return r.is_ok ? r.ok : fallback;                                    \
}                                                                        \
                                                                         \
/* Asserted extraction (fail loudly if Err) */                            \
static inline value_type                                                 \
Result_##value_type##_##error_type##_unwrap(Result_##value_type##_##error_type r) { \
    assert(r.is_ok && "Called unwrap on an Err result");                 \
    return r.ok;                                                         \
}                                                                        \
                                                                         \
static inline value_type                                                 \
Result_##value_type##_##error_type##_expect(Result_##value_type##_##error_type r, const char *msg) { \
    assert(r.is_ok && msg);                                              \
    return r.ok;                                                         \
}                                                                        \
                                                                         \
/* Functional combinators */                                              \
static inline Result_##value_type##_##error_type                          \
Result_##value_type##_##error_type##_map(Result_##value_type##_##error_type r, value_type (*f)(value_type)) { \
    return r.is_ok ? Result_##value_type##_##error_type##_Ok(f(r.ok)) : r; \
}                                                                        \
                                                                         \
static inline Result_##value_type##_##error_type                          \
Result_##value_type##_##error_type##_and_then(Result_##value_type##_##error_type r, \
    Result_##value_type##_##error_type (*f)(value_type)) {               \
    return r.is_ok ? f(r.ok) : r;                                        \
}

#ifdef __cplusplus
}
#endif

#endif /* CANON_C_SEMANTICS_RESULT_H */
