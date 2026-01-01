#ifndef CANON_C_DATA_VEC_H
#define CANON_C_DATA_VEC_H

#include <stddef.h>
#include <stdbool.h>
#include "semantics/result.h"  /* Use your Result<T, E> */
#ifdef __cplusplus
extern "C" {
#endif

/*
    vec.h — bounded dynamic sequence (shape only)

    Vec represents:
    - a contiguous sequence of elements (generic via void*)
    - explicit length
    - explicit capacity

    Vec does NOT:
    - allocate memory
    - resize itself
    - free memory
    - assume ownership

    All memory is owned and managed by the caller.
*/

/* ============================================================
   Generic Vec type (void* version)
   ============================================================ */
typedef struct {
    void   **items;
    size_t   len;
    size_t   capacity;
} Vec;

/* ============================================================
   Result-based push/pop (explicit failure)
   ============================================================ */
CANON_C_DEFINE_RESULT(void*, const char*)  /* Result<void*, const char*> for Vec ops */

/* Initialize Vec with caller-provided buffer */
static inline Vec vec_init(void **buffer, size_t capacity) {
    Vec v;
    v.items = buffer;
    v.len = 0;
    v.capacity = capacity;
    return v;
}

/* Initialize empty Vec (invalid until buffer is set) */
static inline Vec vec_empty(void) {
    Vec v = {0};
    return v;
}

/* Capacity & state */
static inline bool vec_is_empty(const Vec *v) { return !v || v->len == 0; }
static inline bool vec_is_full(const Vec *v)  { return v && v->len >= v->capacity; }
static inline size_t vec_len(const Vec *v)    { return v ? v->len : 0; }
static inline size_t vec_capacity(const Vec *v) { return v ? v->capacity : 0; }

/* Element access */
static inline bool vec_get(const Vec *v, size_t index, void **out) {
    if (!v || !out || !v->items || index >= v->len) return false;
    *out = v->items[index];
    return true;
}

static inline void *vec_get_unchecked(const Vec *v, size_t index) {
    return v->items[index];
}

/* Mutation using Result for explicit failure */
static inline Result_void_ptr_const_char_ptr vec_push(Vec *v, void *item) {
    if (!v || !v->items)
        return Result_void_ptr_const_char_ptr_Err("Vec pointer or buffer NULL");
    if (v->len >= v->capacity)
        return Result_void_ptr_const_char_ptr_Err("Vec capacity exceeded");
    v->items[v->len++] = item;
    return Result_void_ptr_const_char_ptr_Ok(NULL);
}

static inline Result_void_ptr_const_char_ptr vec_pop(Vec *v, void **out) {
    if (!v || !out || !v->items)
        return Result_void_ptr_const_char_ptr_Err("Vec pointer or buffer NULL");
    if (v->len == 0)
        return Result_void_ptr_const_char_ptr_Err("Vec underflow");
    *out = v->items[--v->len];
    return Result_void_ptr_const_char_ptr_Ok(NULL);
}

/* Reset */
static inline void vec_clear(Vec *v) {
    if (v) v->len = 0;
}

/* ============================================================
   Typed Vec macro (optional)
   ============================================================ */
#define DEFINE_VEC(type) \
typedef struct { \
    type *items; \
    size_t len, capacity; \
} Vec_##type; \
\
static inline Vec_##type Vec_##type##_init(type *buffer, size_t capacity) { \
    Vec_##type v = { buffer, 0, capacity }; \
    return v; \
} \
\
static inline bool Vec_##type##_is_empty(const Vec_##type *v) { return !v || v->len == 0; } \
static inline bool Vec_##type##_is_full(const Vec_##type *v)  { return v && v->len >= v->capacity; } \
static inline size_t Vec_##type##_len(const Vec_##type *v)    { return v ? v->len : 0; } \
static inline size_t Vec_##type##_capacity(const Vec_##type *v) { return v ? v->capacity : 0; } \
static inline bool Vec_##type##_get(const Vec_##type *v, size_t i, type *out) { \
    if (!v || !out || i >= v->len) return false; \
    *out = v->items[i]; \
    return true; \
} \
static inline type Vec_##type##_get_unchecked(const Vec_##type *v, size_t i) { return v->items[i]; } \
static inline void Vec_##type##_clear(Vec_##type *v) { if(v) v->len=0; } \
\
/* Typed push/pop with explicit Result */ \
static inline Result_void_ptr_const_char_ptr Vec_##type##_push(Vec_##type *v, type item) { \
    if (!v || !v->items) return Result_void_ptr_const_char_ptr_Err("Vec pointer or buffer NULL"); \
    if (v->len >= v->capacity) return Result_void_ptr_const_char_ptr_Err("Vec capacity exceeded"); \
    v->items[v->len++] = item; \
    return Result_void_ptr_const_char_ptr_Ok(NULL); \
} \
\
static inline Result_void_ptr_const_char_ptr Vec_##type##_pop(Vec_##type *v, type *out) { \
    if (!v || !v->items || !out) return Result_void_ptr_const_char_ptr_Err("Vec pointer or buffer NULL"); \
    if (v->len == 0) return Result_void_ptr_const_char_ptr_Err("Vec underflow"); \
    *out = v->items[--v->len]; \
    return Result_void_ptr_const_char_ptr_Ok(NULL); \
}

#ifdef __cplusplus
}
#endif

#endif /* CANON_C_DATA_VEC_H */
