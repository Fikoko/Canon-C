#ifndef CANON_C_DATA_VEC_H
#define CANON_C_DATA_VEC_H

#include <stddef.h>
#include <stdbool.h>

/*
    vec.h — bounded dynamic sequence (shape only)

    Vec represents:
    - a contiguous sequence of pointers
    - explicit length
    - explicit capacity

    Vec does NOT:
    - allocate memory
    - resize itself
    - free memory
    - assume ownership semantics

    All memory is owned and managed by the caller.
*/

typedef struct {
    void   **items;
    size_t   len;
    size_t   capacity;
} Vec;

/* ============================================================
   Initialization
   ============================================================ */

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
    Vec v;
    v.items = NULL;
    v.len = 0;
    v.capacity = 0;
    return v;
}

/* ============================================================
   Capacity & state
   ============================================================ */

static inline bool vec_is_empty(const Vec *v) {
    return v->len == 0;
}

static inline bool vec_is_full(const Vec *v) {
    return v->len >= v->capacity;
}

static inline size_t vec_len(const Vec *v) {
    return v->len;
}

static inline size_t vec_capacity(const Vec *v) {
    return v->capacity;
}

/* ============================================================
   Element access
   ============================================================ */

/* Get element at index (unchecked) */
static inline void *vec_get_unchecked(const Vec *v, size_t index) {
    return v->items[index];
}

/* Get element at index (checked) */
static inline bool vec_get(const Vec *v, size_t index, void **out) {
    if (!v || !out) return false;
    if (index >= v->len) return false;

    *out = v->items[index];
    return true;
}

/* ============================================================
   Mutation (bounded, explicit failure)
   ============================================================ */

/* Push element, fails if capacity exceeded */
static inline bool vec_push(Vec *v, void *item) {
    if (!v) return false;
    if (v->len >= v->capacity) return false;

    v->items[v->len++] = item;
    return true;
}

/* Pop element, explicit success/failure */
static inline bool vec_pop(Vec *v, void **out) {
    if (!v || !out) return false;
    if (v->len == 0) return false;

    *out = v->items[--v->len];
    return true;
}

/* ============================================================
   Reset
   ============================================================ */

/* Clear vector without touching memory */
static inline void vec_clear(Vec *v) {
    if (!v) return;
    v->len = 0;
}

#endif /* CANON_C_DATA_VEC_H */
