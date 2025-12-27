#include <stdio.h>
#include <canon/canon.h>

void print_int(void *element, void *context) {
    (void)context;
    printf("%d ", *(int *)element);
}

void double_value(void *element, void *context) {
    (void)context;
    *(int *)element *= 2;
}

bool is_even(const void *element, void *context) {
    (void)context;
    return *(const int *)element % 2 == 0;
}

void sum_reduce(void *accumulator, const void *element, void *context) {
    (void)context;
    *(int *)accumulator += *(const int *)element;
}

int main(void) {
    printf("=== Canon Vector Example ===\n\n");
    
    Vector *vec = vector_new(sizeof(int));
    if (!vec) {
        fprintf(stderr, "Failed to create vector\n");
        return 1;
    }
    
    printf("1. Building a vector without manual allocation:\n");
    for (int i = 1; i <= 10; i++) {
        vector_push(vec, &i);
    }
    
    printf("   Vector contents: ");
    vector_foreach(vec, print_int, NULL);
    printf("\n   Size: %zu\n\n", vector_size(vec));
    
    printf("2. Mapping operation (doubling each element):\n");
    vector_map(vec, double_value, NULL);
    printf("   After doubling: ");
    vector_foreach(vec, print_int, NULL);
    printf("\n\n");
    
    printf("3. Filtering operation (keeping only even numbers):\n");
    Vector *evens = vector_filter(vec, is_even, NULL);
    printf("   Even numbers: ");
    vector_foreach(evens, print_int, NULL);
    printf("\n\n");
    
    printf("4. Reducing operation (sum of all elements):\n");
    int sum = 0;
    vector_reduce(vec, &sum, sum_reduce, NULL);
    printf("   Sum: %d\n\n", sum);
    
    printf("5. Direct element access:\n");
    int *elem = vector_get(vec, 0);
    if (elem) {
        printf("   First element: %d\n", *elem);
    }
    elem = vector_get(vec, vector_size(vec) - 1);
    if (elem) {
        printf("   Last element: %d\n\n", *elem);
    }
    
    vector_free(evens);
    vector_free(vec);
    
    printf("Memory automatically freed - no manual bookkeeping!\n");
    
    return 0;
}
