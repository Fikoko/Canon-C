#ifndef CANON_C_DATA_DEQUE_H
#define CANON_C_DATA_DEQUE_H

#include <stddef.h>
#include <stdbool.h>

/*
    deque.h — Bounded double-ended queue (ring buffer over Vec-like storage)
*/

#define DEFINE_DEQUE(Type) \
typedef struct { \
    Type* buffer; \
    size_t capacity; \
    size_t head; \
    size_t tail; \
    size_t size; \
} deque_##Type; \
\
static inline void deque_##Type##_init(deque_##Type* d, Type* buffer, size_t capacity) \
{ \
    if (d && buffer && capacity > 0) { \
        *d = (deque_##Type){ .buffer = buffer, .capacity = capacity, .head = 0, .tail = 0, .size = 0 }; \
    } \
} \
\
static inline bool deque_##Type##_push_front(deque_##Type* d, Type item) \
{ \
    if (!d || d->size >= d->capacity) return false; \
    d->head = (d->head == 0) ? d->capacity - 1 : d->head - 1; \
    d->buffer[d->head] = item; \
    d->size++; \
    return true; \
} \
\
static inline bool deque_##Type##_push_back(deque_##Type* d, Type item) \
{ \
    if (!d || d->size >= d->capacity) return false; \
    d->buffer[d->tail] = item; \
    d->tail = (d->tail + 1) % d->capacity; \
    d->size++; \
    return true; \
} \
\
static inline bool deque_##Type##_pop_front(deque_##Type* d, Type* out) \
{ \
    if (!d || !out || d->size == 0) return false; \
    *out = d->buffer[d->head]; \
    d->head = (d->head + 1) % d->capacity; \
    d->size--; \
    return true; \
} \
\
static inline bool deque_##Type##_pop_back(deque_##Type* d, Type* out) \
{ \
    if (!d || !out || d->size == 0) return false; \
    d->tail = (d->tail == 0) ? d->capacity - 1 : d->tail - 1; \
    *out = d->buffer[d->tail]; \
    d->size--; \
    return true; \
} \
\
static inline size_t deque_##Type##_size(const deque_##Type* d) { return d ? d->size : 0; } \
static inline bool deque_##Type##_empty(const deque_##Type* d) { return deque_##Type##_size(d) == 0; }

#endif /* CANON_C_DATA_DEQUE_H */
