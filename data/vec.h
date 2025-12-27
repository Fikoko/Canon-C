#ifndef CANON_C_DATA_VEC_H
#define CANON_C_DATA_VEC_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    void** items;
    size_t len;
    size_t capacity;
} Vec;

// Initialize vector
static inline Vec vec_init(size_t capacity) {
    Vec v;
    v.items = (void**)malloc(sizeof(void*) * capacity);
    v.len = 0;
    v.capacity = capacity;
    return v;
}

// Push item to vector
static inline bool vec_push(Vec* v, void* item) {
    if (v->len >= v->capacity) {
        size_t new_capacity = v->capacity * 2 + 1;
        void** new_items = (void**)realloc(v->items, sizeof(void*) * new_capacity);
        if (!new_items) return false;
        v->items = new_items;
        v->capacity = new_capacity;
    }
    v->items[v->len++] = item;
    return true;
}

// Pop last item from vector
static inline void* vec_pop(Vec* v) {
    if (v->len == 0) return NULL;
    return v->items[--v->len];
}

// Get item by index (NULL if out-of-bounds)
static inline void* vec_get(Vec* v, size_t index) {
    if (index >= v->len) return NULL;
    return v->items[index];
}

// Free vector
static inline void vec_free(Vec* v) {
    free(v->items);
    v->items = NULL;
    v->len = 0;
    v->capacity = 0;
}

#endif // CANON_C_DATA_VEC_H
