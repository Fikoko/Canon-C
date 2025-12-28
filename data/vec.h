#ifndef CANON_C_DATA_VEC_H
#define CANON_C_DATA_VEC_H

#include <stddef.h>
#include <stdbool.h>
#include <canon/core/memory.h>

/*
    vec.h — dynamic vector (shape, not policy)

    Vec manages:
    - length
    - capacity
    - indexing

    Vec does NOT decide:
    - where memory comes from
    - how memory is freed (except in heap mode)
*/

typedef struct {
    void   **items;
    size_t   len;
    size_t   capacity;
} Vec;

/* ============================================================
   Heap-backed (automatic mode)
   ============================================================ */

/* Initialize vector with heap allocation */
static inline Vec vec_init_heap(size_t capacity) {
    Vec v;
    v.items = (void **)mem_alloc(sizeof(void *) * capacity);
    v.len = 0;
    v.capacity = v.items ? capacity : 0;
    return v;
}

/* Push item, growing automatically */
static inline bool vec_push(Vec *v, void *item) {
    if (v->len >= v->capacity) {
        size_t new_capacity = v->capacity ? (v->capacity * 2) : 1;
        void **new_items =
            (void **)mem_realloc(v->items, sizeof(void *) * new_capacity);

        if (!new_items)
            return false;

        v->items = new_items;
        v->capacity = new_capacity;
    }

    v->items[v->len++] = item;
    return true;
}

/* Pop last item */
static inline void *vec_pop(Vec *v) {
    if (v->len == 0)
        return NULL;

    return v->items[--v->len];
}

/* Free heap-backed vector */
static inline void vec_free_heap(Vec *v) {
    mem_free(v->items);
    v->items = NULL;
    v->len = 0;
    v->capacity = 0;
}

/* ============================================================
   Manual buffer-backed mode
   ============================================================ */

/* Initialize vector with caller-provided buffer */
static inline Vec vec_init_buffer(void **buffer, size_t capacity) {
    Vec v;
    v.items = buffer;
    v.len = 0;
    v.capacity = capacity;
    return v;
}

/* Push without resizing, fails if full */
static inline bool vec_push_manual(Vec *v, void *item) {
    if (v->len >= v->capacity)
        return false;

    v->items[v->len++] = item;
    return true;
}

#endif /* CANON_C_DATA_VEC_H */
