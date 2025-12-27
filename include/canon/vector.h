#ifndef CANON_VECTOR_H
#define CANON_VECTOR_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
} Vector;

Vector *vector_new(size_t element_size);
void vector_free(Vector *vec);
bool vector_push(Vector *vec, const void *element);
bool vector_pop(Vector *vec, void *out);
void *vector_get(const Vector *vec, size_t index);
bool vector_set(Vector *vec, size_t index, const void *element);
size_t vector_size(const Vector *vec);
bool vector_is_empty(const Vector *vec);
void vector_clear(Vector *vec);
bool vector_reserve(Vector *vec, size_t capacity);

typedef void (*VectorMapFn)(void *element, void *context);
typedef bool (*VectorFilterFn)(const void *element, void *context);
typedef void (*VectorReduceFn)(void *accumulator, const void *element, void *context);

void vector_map(Vector *vec, VectorMapFn fn, void *context);
Vector *vector_filter(const Vector *vec, VectorFilterFn fn, void *context);
void vector_reduce(const Vector *vec, void *accumulator, VectorReduceFn fn, void *context);
void vector_foreach(const Vector *vec, VectorMapFn fn, void *context);

#endif
