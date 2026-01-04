#ifndef CANON_C_DATA_QUEUE_H
#define CANON_C_DATA_QUEUE_H

#include "deque.h"

/*
    queue.h — FIFO queue (thin wrapper over deque)
*/

#define DEFINE_QUEUE(Type) \
DEFINE_DEQUE(Type) \
\
typedef deque_##Type queue_##Type; \
\
static inline void queue_##Type##_init(queue_##Type* q, Type* buffer, size_t capacity) \
{ \
    deque_##Type##_init(q, buffer, capacity); \
} \
\
static inline bool queue_##Type##_enqueue(queue_##Type* q, Type item) \
{ \
    return deque_##Type##_push_back(q, item); \
} \
\
static inline bool queue_##Type##_dequeue(queue_##Type* q, Type* out) \
{ \
    return deque_##Type##_pop_front(q, out); \
}

#endif /* CANON_C_DATA_QUEUE_H */
