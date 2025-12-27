#ifndef CANON_STRING_UTILS_H
#define CANON_STRING_UTILS_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} String;

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

typedef struct {
    char **strings;
    size_t count;
} StringArray;

StringArray *string_split(const char *str, const char *delimiter);
void string_array_free(StringArray *arr);
String *string_join(const StringArray *arr, const char *separator);

#endif
