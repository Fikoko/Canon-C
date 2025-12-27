#include "canon/vector.h"
#include <stdlib.h>
#include <string.h>

#define VECTOR_INITIAL_CAPACITY 8
#define VECTOR_GROWTH_FACTOR 2

Vector *vector_new(size_t element_size) {
    Vector *vec = malloc(sizeof(Vector));
    if (!vec) return NULL;
    
    vec->data = malloc(VECTOR_INITIAL_CAPACITY * element_size);
    if (!vec->data) {
        free(vec);
        return NULL;
    }
    
    vec->size = 0;
    vec->capacity = VECTOR_INITIAL_CAPACITY;
    vec->element_size = element_size;
    return vec;
}

void vector_free(Vector *vec) {
    if (!vec) return;
    free(vec->data);
    free(vec);
}

bool vector_reserve(Vector *vec, size_t capacity) {
    if (!vec || capacity <= vec->capacity) return true;
    
    void *new_data = realloc(vec->data, capacity * vec->element_size);
    if (!new_data) return false;
    
    vec->data = new_data;
    vec->capacity = capacity;
    return true;
}

bool vector_push(Vector *vec, const void *element) {
    if (!vec || !element) return false;
    
    if (vec->size >= vec->capacity) {
        size_t new_capacity = vec->capacity * VECTOR_GROWTH_FACTOR;
        if (!vector_reserve(vec, new_capacity)) {
            return false;
        }
    }
    
    void *dest = (char *)vec->data + (vec->size * vec->element_size);
    memcpy(dest, element, vec->element_size);
    vec->size++;
    return true;
}

bool vector_pop(Vector *vec, void *out) {
    if (!vec || vec->size == 0) return false;
    
    vec->size--;
    if (out) {
        void *src = (char *)vec->data + (vec->size * vec->element_size);
        memcpy(out, src, vec->element_size);
    }
    return true;
}

void *vector_get(const Vector *vec, size_t index) {
    if (!vec || index >= vec->size) return NULL;
    return (char *)vec->data + (index * vec->element_size);
}

bool vector_set(Vector *vec, size_t index, const void *element) {
    if (!vec || !element || index >= vec->size) return false;
    
    void *dest = (char *)vec->data + (index * vec->element_size);
    memcpy(dest, element, vec->element_size);
    return true;
}

size_t vector_size(const Vector *vec) {
    return vec ? vec->size : 0;
}

bool vector_is_empty(const Vector *vec) {
    return vec ? (vec->size == 0) : true;
}

void vector_clear(Vector *vec) {
    if (vec) vec->size = 0;
}

void vector_map(Vector *vec, VectorMapFn fn, void *context) {
    if (!vec || !fn) return;
    
    for (size_t i = 0; i < vec->size; i++) {
        void *element = (char *)vec->data + (i * vec->element_size);
        fn(element, context);
    }
}

Vector *vector_filter(const Vector *vec, VectorFilterFn fn, void *context) {
    if (!vec || !fn) return NULL;
    
    Vector *result = vector_new(vec->element_size);
    if (!result) return NULL;
    
    for (size_t i = 0; i < vec->size; i++) {
        void *element = (char *)vec->data + (i * vec->element_size);
        if (fn(element, context)) {
            if (!vector_push(result, element)) {
                vector_free(result);
                return NULL;
            }
        }
    }
    
    return result;
}

void vector_reduce(const Vector *vec, void *accumulator, VectorReduceFn fn, void *context) {
    if (!vec || !fn || !accumulator) return;
    
    for (size_t i = 0; i < vec->size; i++) {
        void *element = (char *)vec->data + (i * vec->element_size);
        fn(accumulator, element, context);
    }
}

void vector_foreach(const Vector *vec, VectorMapFn fn, void *context) {
    if (!vec || !fn) return;
    
    for (size_t i = 0; i < vec->size; i++) {
        void *element = (char *)vec->data + (i * vec->element_size);
        fn(element, context);
    }
}
