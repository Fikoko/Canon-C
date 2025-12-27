#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>

typedef void* (*MapFunc)(void*);

// Apply function to each element in array (in-place)
static inline void map(void** items, size_t len, MapFunc f) {
    for (size_t i = 0; i < len; i++) {
        items[i] = f(items[i]);
    }
}

// Optional: map to new array
static inline void** map_new(void** items, size_t len, MapFunc f) {
    void** new_items = (void**)malloc(sizeof(void*) * len);
    if (!new_items) return NULL;
    for (size_t i = 0; i < len; i++) {
        new_items[i] = f(items[i]);
    }
    return new_items;
}

#endif // CANON_C_ALGO_MAP_H
