# Canon C Library

A collection of small, explicit C utilities that form a coherent semantic layer on top of C.

## Philosophy

The goal is not to extend C into another language, but to reduce the semantic and cognitive burden of programming by replacing low-level mechanics (manual allocation, pointer bookkeeping, loop scaffolding) with meaningful, functional-style operations.

**C is treated here as a portable, predictable execution backend. Semantics live in the library.**

## Features

- **Dynamic Arrays (Vectors)**: Automatic memory management for collections
- **String Utilities**: Safe string manipulation without buffer overflows
- **Functional Operations**: Map, filter, reduce operations on collections
- **Option Types**: Explicit handling of presence/absence of values
- **Result Types**: Type-safe error handling without manual error codes
- **Zero Dependencies**: Pure C11, no external dependencies

## Building

```bash
make              # Build the library
make examples     # Build the example programs
make clean        # Clean build artifacts
```

The library will be built as `build/libcanon.a`.

## Installation

```bash
sudo make install
```

This installs:
- Library to `/usr/local/lib/libcanon.a`
- Headers to `/usr/local/include/canon/`

## Quick Start

```c
#include <canon/canon.h>

int main(void) {
    // Create a dynamic vector (no manual allocation!)
    Vector *numbers = vector_new(sizeof(int));
    
    for (int i = 1; i <= 5; i++) {
        vector_push(numbers, &i);
    }
    
    // Safe string operations
    String *message = string_new("Hello, ");
    string_append(message, "World!");
    printf("%s\n", string_cstr(message));
    
    // Clean up (simple!)
    vector_free(numbers);
    string_free(message);
    
    return 0;
}
```

Compile with:
```bash
gcc -Iinclude myprogram.c -Lbuild -lcanon -o myprogram
```

## Core Components

### Vector (Dynamic Array)

Automatic memory management for growable arrays of any type.

**Benefits:**
- No manual realloc tracking
- Type-generic storage
- Functional operations (map, filter, reduce)

**Example:**
```c
Vector *vec = vector_new(sizeof(int));

// Add elements
int values[] = {1, 2, 3, 4, 5};
for (int i = 0; i < 5; i++) {
    vector_push(vec, &values[i]);
}

// Functional operations
vector_map(vec, double_value, NULL);        // Transform
Vector *evens = vector_filter(vec, is_even, NULL);  // Filter
vector_reduce(vec, &sum, accumulate, NULL);  // Reduce

vector_free(vec);
vector_free(evens);
```

### String Utilities

Safe, dynamic string handling.

**Benefits:**
- Automatic growth
- No buffer overflow risks
- Rich manipulation API

**Example:**
```c
String *str = string_new("Hello");
string_append(str, " World");
string_append_char(str, '!');

StringArray *words = string_split(string_cstr(str), " ");
String *joined = string_join(words, " | ");

string_free(str);
string_free(joined);
string_array_free(words);
```

### Option Types

Explicit handling of values that may or may not be present.

**Benefits:**
- No implicit NULL checking
- Self-documenting APIs
- Type-safe optional values

**Example:**
```c
typedef OPTION(int) OptionInt;

OptionInt find_value(int *array, size_t size, int target) {
    for (size_t i = 0; i < size; i++) {
        if (array[i] == target) {
            return SOME(array[i]);
        }
    }
    return NONE;
}

OptionInt result = find_value(numbers, 5, 3);
if (option_is_some(result)) {
    printf("Found: %d\n", option_unwrap(result));
} else {
    printf("Not found\n");
}
```

### Result Types

Explicit error handling without error codes.

**Benefits:**
- Forces error handling
- Self-documenting error cases
- Type-safe errors

**Example:**
```c
typedef RESULT(int, const char*) ResultInt;

ResultInt safe_divide(int a, int b) {
    if (b == 0) {
        return ERR("Division by zero");
    }
    return OK(a / b);
}

ResultInt result = safe_divide(10, 2);
if (result_is_ok(result)) {
    printf("Result: %d\n", result_unwrap(result));
} else {
    printf("Error: %s\n", result_unwrap_err(result));
}
```

## Examples

The `examples/` directory contains fully working demonstrations:

1. **vector_example.c** - Dynamic arrays and functional operations
2. **string_example.c** - String manipulation and utilities
3. **option_result_example.c** - Type-safe error handling
4. **comprehensive_example.c** - Student management system combining all features

Run any example:
```bash
make examples
./build/vector_example
./build/string_example
./build/option_result_example
./build/comprehensive_example
```

## API Reference

### Vector API

```c
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

// Functional operations
void vector_map(Vector *vec, VectorMapFn fn, void *context);
Vector *vector_filter(const Vector *vec, VectorFilterFn fn, void *context);
void vector_reduce(const Vector *vec, void *accumulator, VectorReduceFn fn, void *context);
void vector_foreach(const Vector *vec, VectorMapFn fn, void *context);
```

### String API

```c
String *string_new(const char *initial);
String *string_new_with_capacity(size_t capacity);
void string_free(String *str);
bool string_append(String *str, const char *text);
bool string_append_char(String *str, char c);
const char *string_cstr(const String *str);
size_t string_length(const String *str);
void string_clear(String *str);
String *string_clone(const String *str);
bool string_equals(const String *a, const String *b);
String *string_substring(const String *str, size_t start, size_t length);

StringArray *string_split(const char *str, const char *delimiter);
void string_array_free(StringArray *arr);
String *string_join(const StringArray *arr, const char *separator);
```

### Option API

```c
// Macro definitions
OPTION(T)                           // Define an option type
SOME(val)                          // Create Some value
NONE                               // Create None value
option_is_some(opt)                // Check if option has a value
option_is_none(opt)                // Check if option is empty
option_unwrap(opt)                 // Get value (unsafe if None)
option_unwrap_or(opt, default)     // Get value or default

// Pointer helpers
OptionPtr option_ptr_some(void *value);
OptionPtr option_ptr_none(void);
```

### Result API

```c
// Macro definitions
RESULT(T, E)                       // Define a result type
OK(val)                            // Create Ok result
ERR(val)                           // Create Err result
result_is_ok(res)                  // Check if result is Ok
result_is_err(res)                 // Check if result is Err
result_unwrap(res)                 // Get Ok value (unsafe if Err)
result_unwrap_err(res)             // Get Err value (unsafe if Ok)
result_unwrap_or(res, default)     // Get value or default

// Pointer helpers
ResultPtr result_ptr_ok(void *value);
ResultPtr result_ptr_err(const char *error);
```

## Design Principles

1. **Explicit over implicit**: All operations are clear and self-documenting
2. **Safe by default**: Memory is managed, bounds are checked
3. **Functional style**: Encourage map/filter/reduce over manual loops
4. **Type-aware**: Strong typing with minimal casting
5. **Zero-cost abstractions**: Thin wrappers over standard C
6. **Minimal footprint**: Small, focused library with no dependencies

## Comparison: Before and After

### Manual Allocation vs Vector

**Before:**
```c
int *array = malloc(10 * sizeof(int));
size_t size = 0, capacity = 10;

// Add element
if (size >= capacity) {
    capacity *= 2;
    int *new_array = realloc(array, capacity * sizeof(int));
    if (!new_array) { /* handle error */ }
    array = new_array;
}
array[size++] = value;

free(array);
```

**After:**
```c
Vector *vec = vector_new(sizeof(int));
vector_push(vec, &value);
vector_free(vec);
```

### Manual Loop vs Functional Operations

**Before:**
```c
for (size_t i = 0; i < size; i++) {
    array[i] *= 2;  // Manual iteration
}
```

**After:**
```c
vector_map(vec, double_value, NULL);  // Semantic intent clear
```

### NULL Checks vs Option Types

**Before:**
```c
int *result = find_value(array, size, target);
if (result != NULL) {
    printf("Found: %d\n", *result);
} else {
    printf("Not found\n");
}
```

**After:**
```c
OptionInt result = find_value(array, size, target);
if (option_is_some(result)) {
    printf("Found: %d\n", option_unwrap(result));
} else {
    printf("Not found\n");
}
```

## License

GNU General Public License v3.0 - see LICENSE file for details.

## Contributing

Contributions are welcome! Please focus on:
- Maintaining the zero-dependency philosophy
- Adding utilities that reduce cognitive burden
- Keeping implementations simple and portable
- Providing clear examples and documentation
