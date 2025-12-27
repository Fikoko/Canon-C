#ifndef CANON_C_ALGO_MAP_H
#define CANON_C_ALGO_MAP_H

#include <stddef.h>
#include <stdlib.h>

typedef void* (*MapFunc)(void*);

// --- Automatic mode ---

static inline void map(void** items, size_t len, MapFunc f) {
    for (size_t i = 0; i < len; i++) {
        items[i] = f(items[i]);
    }
}

static inline void** map_new(void** items, size_t len, MapFunc f) {
    void** new_items = (void**)malloc(sizeof(void*) * len);
    if (!new_items) return NULL;
    for (size_t i = 0; i < len; i++) {
        new_items[i] = f(items[i]);
    }
    return new_items;
}

// --- Manual mode ---

// Map into preallocated buffer
static inline void map_manual(void** items, size_t len, MapFunc f, void** output) {
    for (size_t i = 0; i < len; i++) {
        output[i] = f(items[i]);
    }
}

#endif // CANON_C_ALGO_MAP_H
