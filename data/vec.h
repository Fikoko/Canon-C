#ifndef CANON_C_DATA_VEC_H
#define CANON_C_DATA_VEC_H

#include <stddef.h>
#include <stdbool.h>
#include "core/memory.h"      // for mem_copy, etc. if needed later
#include "semantics/result.h"

/*
    vec.h — Bounded dynamic vector (explicit buffer, no ownership)

    Vec<T> is a contiguous sequence with:
      - explicit length
      - explicit capacity
      - caller-owned buffer

    No allocation, no resizing, no freeing.
    All operations are bounded and explicit.
    Push/pop return Result for safe, composable error handling.
*/

#define CANON_C_DEFINE_RESULT_UNIT(error_type) \
    CANON_C_DEFINE_RESULT(bool, error_type)  // true = Ok(()), false not used

CANON_C_DEFINE_RESULT_UNIT(const char*)  // Result<bool, const char*> ≈ Result<(), const char*>

/* ============================================================
   Generic Vec (void*)
   ============================================================ */

typedef struct {
    void** items;
    size_t len;
    size_t capacity;
} vec_voidp;

/* Initialization */
static inline vec_voidp vec_voidp_init(void** buffer, size_t capacity)
{
    return (vec_voidp){ .items = buffer, .len = 0, .capacity = capacity };
}

static inline vec_voidp vec_voidp_empty(void)
{
    return (vec_voidp){0};
}

/* Queries */
static inline bool      vec_voidp_is_empty(const vec_voidp* v) { return !v || v->len == 0; }
static inline bool      vec_voidp_is_full(const vec_voidp* v)  { return v && v->len >= v->capacity; }
static inline size_t    vec_voidp_len(const vec_voidp* v)      { return v ? v->len : 0; }
static inline size_t    vec_voidp_capacity(const vec_voidp* v){ return v ? v->capacity : 0; }

/* Safe access */
static inline bool vec_voidp_get(const vec_voidp* v, size_t i, void** out)
{
    if (!v || !out || i >= v->len) return false;
    *out = v->items[i];
    return true;
}

static inline void* vec_voidp_get_unchecked(const vec_voidp* v, size_t i)
{
    return v->items[i];
}

/* Mutation with explicit failure */
static inline result_bool_constcharp vec_voidp_push(vec_voidp* v, void* item)
{
    if (!v || !v->items)
        return result_bool_constcharp_err("null vec or buffer");
    if (v->len >= v->capacity)
        return result_bool_constcharp_err("capacity exceeded");
    v->items[v->len++] = item;
    return result_bool_constcharp_ok(true);
}

static inline result_bool_constcharp vec_voidp_pop(vec_voidp* v, void** out)
{
    if (!v || !out || !v->items)
        return result_bool_constcharp_err("null vec or buffer");
    if (v->len == 0)
        return result_bool_constcharp_err("pop from empty vec");
    *out = v->items[--v->len];
    return result_bool_constcharp_ok(true);
}

static inline void vec_voidp_clear(vec_voidp* v)
{
    if (v) v->len = 0;
}

/* ============================================================
   Typed Vec Macro (recommended usage)
   ============================================================ */

#define DEFINE_VEC(type) \
typedef struct { \
    type* items; \
    size_t len; \
    size_t capacity; \
} vec_##type; \
\
static inline vec_##type vec_##type##_init(type* buffer, size_t capacity) \
{ \
    return (vec_##type){ .items = buffer, .len = 0, .capacity = capacity }; \
} \
\
static inline vec_##type vec_##type##_empty(void) \
{ \
    return (vec_##type){0}; \
} \
\
static inline bool vec_##type##_is_empty(const vec_##type* v) { return !v || v->len == 0; } \
static inline bool vec_##type##_is_full(const vec_##type* v)  { return v && v->len >= v->capacity; } \
static inline size_t vec_##type##_len(const vec_##type* v)    { return v ? v->len : 0; } \
static inline size_t vec_##type##_capacity(const vec_##type* v){ return v ? v->capacity : 0; } \
\
static inline bool vec_##type##_get(const vec_##type* v, size_t i, type* out) \
{ \
    if (!v || !out || i >= v->len) return false; \
    *out = v->items[i]; \
    return true; \
} \
\
static inline type vec_##type##_get_unchecked(const vec_##type* v, size_t i) \
{ \
    return v->items[i]; \
} \
\
static inline result_bool_constcharp vec_##type##_push(vec_##type* v, type item) \
{ \
    if (!v || !v->items) return result_bool_constcharp_err("null vec or buffer"); \
    if (v->len >= v->capacity) return result_bool_constcharp_err("capacity exceeded"); \
    v->items[v->len++] = item; \
    return result_bool_constcharp_ok(true); \
} \
\
static inline result_bool_constcharp vec_##type##_pop(vec_##type* v, type* out) \
{ \
    if (!v || !out || !v->items) return result_bool_constcharp_err("null vec or buffer"); \
    if (v->len == 0) return result_bool_constcharp_err("pop from empty vec"); \
    *out = v->items[--v->len]; \
    return result_bool_constcharp_ok(true); \
} \
\
static inline void vec_##type##_clear(vec_##type* v) \
{ \
    if (v) v->len = 0; \
}

#endif /* CANON_C_DATA_VEC_H */
