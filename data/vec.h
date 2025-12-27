#ifndef CANON_C_DATA_VEC_H
#define CANON_C_DATA_VEC_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    void** items;
    size_t len;
    size_t capacity;
} Vec;

// --- Automatic mode ---

static inline Vec vec_init(size_t capacity) {
    Vec v;
    v.items = (void**)malloc(sizeof(void*) * capacity);
    v.len = 0;
    v.capacity = capacity;
    return v;
}

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

// Pop last item (automatic)
static inline void* vec_pop(Vec* v) {
    if (v->len == 0) return NULL;
    return v->items[--v->len];
}

// --- Manual mode variants ---

// Push with no automatic resize, fails if full
static inline bool vec_push_manual(Vec* v, void* item) {
    if (v->len >= v->capacity) return false;
    v->items[v->len++] = item;
    return true;
}

// Initialize with preallocated buffer (manual)
static inline Vec vec_init_manual(void** buffer, size_t capacity) {
    Vec v;
    v.items = buffer;
    v.len = 0;
    v.capacity = capacity;
    return v;
}

// Free vector (same for both modes)
static inline void vec_free(Vec* v) {
    free(v->items);
    v->items = NULL;
    v->len = 0;
    v->capacity = 0;
}

#endif // CANON_C_DATA_VEC_H
