#include <stdio.h>
#include <stdlib.h>
#include <canon/canon.h>

typedef OPTION(int) OptionInt;
typedef RESULT(int, const char*) ResultInt;

OptionInt find_value(int *array, size_t size, int target) {
    for (size_t i = 0; i < size; i++) {
        if (array[i] == target) {
            OptionInt opt = SOME(array[i]);
            return opt;
        }
    }
    OptionInt opt = NONE;
    return opt;
}

ResultInt safe_divide(int a, int b) {
    if (b == 0) {
        ResultInt res = ERR("Division by zero");
        return res;
    }
    ResultInt res = OK(a / b);
    return res;
}

ResultPtr safe_allocate(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        return result_ptr_err("Memory allocation failed");
    }
    return result_ptr_ok(ptr);
}

int main(void) {
    printf("=== Canon Option and Result Types Example ===\n\n");
    
    printf("1. Option type - handling presence/absence of values:\n");
    int numbers[] = {1, 2, 3, 4, 5};
    
    OptionInt result = find_value(numbers, 5, 3);
    if (option_is_some(result)) {
        printf("   Found value: %d\n", option_unwrap(result));
    } else {
        printf("   Value not found\n");
    }
    
    result = find_value(numbers, 5, 10);
    if (option_is_some(result)) {
        printf("   Found value: %d\n", option_unwrap(result));
    } else {
        printf("   Value not found\n");
    }
    
    printf("   Using unwrap_or for default: %d\n\n", 
           option_unwrap_or(result, -1));
    
    printf("2. Result type - explicit error handling:\n");
    ResultInt div_result = safe_divide(10, 2);
    if (result_is_ok(div_result)) {
        printf("   10 / 2 = %d\n", result_unwrap(div_result));
    } else {
        printf("   Error: %s\n", result_unwrap_err(div_result));
    }
    
    div_result = safe_divide(10, 0);
    if (result_is_ok(div_result)) {
        printf("   10 / 0 = %d\n", result_unwrap(div_result));
    } else {
        printf("   Error: %s\n", result_unwrap_err(div_result));
    }
    
    printf("   Using unwrap_or for default: %d\n\n",
           result_unwrap_or(div_result, 0));
    
    printf("3. Result with pointers - safe resource allocation:\n");
    ResultPtr alloc_result = safe_allocate(1024);
    if (result_is_ok(alloc_result)) {
        printf("   Allocation succeeded: %p\n", result_unwrap(alloc_result));
        free(result_unwrap(alloc_result));
    } else {
        printf("   Allocation failed: %s\n", result_unwrap_err(alloc_result));
    }
    
    printf("\nNo manual error code checking or NULL pointer guards needed!\n");
    printf("Errors are explicit and type-safe.\n");
    
    return 0;
}
