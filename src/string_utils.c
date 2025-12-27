#include "canon/string_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STRING_INITIAL_CAPACITY 32
#define STRING_MAX_CAPACITY (SIZE_MAX / 2)

static char *canon_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *new_str = malloc(len);
    if (new_str) {
        memcpy(new_str, s, len);
    }
    return new_str;
}

String *string_new(const char *initial) {
    String *str = malloc(sizeof(String));
    if (!str) return NULL;
    
    size_t len = initial ? strlen(initial) : 0;
    size_t capacity = len + 1;
    if (capacity < STRING_INITIAL_CAPACITY) {
        capacity = STRING_INITIAL_CAPACITY;
    }
    
    str->data = malloc(capacity);
    if (!str->data) {
        free(str);
        return NULL;
    }
    
    if (initial) {
        memcpy(str->data, initial, len);
    }
    str->data[len] = '\0';
    str->length = len;
    str->capacity = capacity;
    
    return str;
}

String *string_new_with_capacity(size_t capacity) {
    String *str = malloc(sizeof(String));
    if (!str) return NULL;
    
    str->data = malloc(capacity);
    if (!str->data) {
        free(str);
        return NULL;
    }
    
    str->data[0] = '\0';
    str->length = 0;
    str->capacity = capacity;
    
    return str;
}

void string_free(String *str) {
    if (!str) return;
    free(str->data);
    free(str);
}

bool string_append(String *str, const char *text) {
    if (!str || !text) return false;
    
    size_t text_len = strlen(text);
    
    if (text_len > SIZE_MAX - str->length - 1) {
        return false;
    }
    
    size_t new_len = str->length + text_len;
    
    if (new_len + 1 > str->capacity) {
        size_t new_capacity = str->capacity;
        
        while (new_capacity <= new_len) {
            if (new_capacity > STRING_MAX_CAPACITY) {
                return false;
            }
            size_t next_capacity = new_capacity * 2;
            if (next_capacity < new_capacity || next_capacity > STRING_MAX_CAPACITY) {
                new_capacity = STRING_MAX_CAPACITY;
                break;
            }
            new_capacity = next_capacity;
        }
        
        char *new_data = realloc(str->data, new_capacity);
        if (!new_data) return false;
        
        str->data = new_data;
        str->capacity = new_capacity;
    }
    
    memcpy(str->data + str->length, text, text_len);
    str->length = new_len;
    str->data[str->length] = '\0';
    
    return true;
}

bool string_append_char(String *str, char c) {
    char buf[2] = {c, '\0'};
    return string_append(str, buf);
}

const char *string_cstr(const String *str) {
    return str ? str->data : NULL;
}

size_t string_length(const String *str) {
    return str ? str->length : 0;
}

void string_clear(String *str) {
    if (!str) return;
    str->length = 0;
    str->data[0] = '\0';
}

String *string_clone(const String *str) {
    if (!str) return NULL;
    return string_new(str->data);
}

bool string_equals(const String *a, const String *b) {
    if (!a || !b) return false;
    if (a->length != b->length) return false;
    return strcmp(a->data, b->data) == 0;
}

String *string_substring(const String *str, size_t start, size_t length) {
    if (!str || start >= str->length) return NULL;
    
    size_t actual_length = length;
    if (start + length > str->length) {
        actual_length = str->length - start;
    }
    
    String *result = string_new_with_capacity(actual_length + 1);
    if (!result) return NULL;
    
    memcpy(result->data, str->data + start, actual_length);
    result->data[actual_length] = '\0';
    result->length = actual_length;
    
    return result;
}

StringArray *string_split(const char *str, const char *delimiter) {
    if (!str || !delimiter) return NULL;
    
    StringArray *arr = malloc(sizeof(StringArray));
    if (!arr) return NULL;
    
    size_t count = 0;
    size_t capacity = 8;
    arr->strings = malloc(sizeof(char *) * capacity);
    if (!arr->strings) {
        free(arr);
        return NULL;
    }
    
    char *str_copy = canon_strdup(str);
    if (!str_copy) {
        free(arr->strings);
        free(arr);
        return NULL;
    }
    
    char *token = strtok(str_copy, delimiter);
    while (token) {
        if (count >= capacity) {
            capacity *= 2;
            char **new_strings = realloc(arr->strings, sizeof(char *) * capacity);
            if (!new_strings) {
                for (size_t i = 0; i < count; i++) {
                    free(arr->strings[i]);
                }
                free(arr->strings);
                free(str_copy);
                free(arr);
                return NULL;
            }
            arr->strings = new_strings;
        }
        
        arr->strings[count] = canon_strdup(token);
        if (!arr->strings[count]) {
            for (size_t i = 0; i < count; i++) {
                free(arr->strings[i]);
            }
            free(arr->strings);
            free(str_copy);
            free(arr);
            return NULL;
        }
        count++;
        token = strtok(NULL, delimiter);
    }
    
    free(str_copy);
    arr->count = count;
    
    return arr;
}

void string_array_free(StringArray *arr) {
    if (!arr) return;
    
    for (size_t i = 0; i < arr->count; i++) {
        free(arr->strings[i]);
    }
    free(arr->strings);
    free(arr);
}

String *string_join(const StringArray *arr, const char *separator) {
    if (!arr || arr->count == 0) return string_new("");
    
    size_t total_len = 0;
    size_t sep_len = separator ? strlen(separator) : 0;
    
    for (size_t i = 0; i < arr->count; i++) {
        total_len += strlen(arr->strings[i]);
        if (i < arr->count - 1) {
            total_len += sep_len;
        }
    }
    
    String *result = string_new_with_capacity(total_len + 1);
    if (!result) return NULL;
    
    for (size_t i = 0; i < arr->count; i++) {
        string_append(result, arr->strings[i]);
        if (i < arr->count - 1 && separator) {
            string_append(result, separator);
        }
    }
    
    return result;
}
