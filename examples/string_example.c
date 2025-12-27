#include <stdio.h>
#include <canon/canon.h>

int main(void) {
    printf("=== Canon String Example ===\n\n");
    
    printf("1. Safe string creation and manipulation:\n");
    String *greeting = string_new("Hello");
    string_append(greeting, ", ");
    string_append(greeting, "World");
    string_append_char(greeting, '!');
    printf("   Result: %s\n", string_cstr(greeting));
    printf("   Length: %zu\n\n", string_length(greeting));
    
    printf("2. String splitting (no manual tokenization):\n");
    const char *sentence = "The quick brown fox jumps";
    StringArray *words = string_split(sentence, " ");
    printf("   Original: %s\n", sentence);
    printf("   Words:\n");
    for (size_t i = 0; i < words->count; i++) {
        printf("     [%zu]: %s\n", i, words->strings[i]);
    }
    printf("\n");
    
    printf("3. String joining:\n");
    String *joined = string_join(words, " | ");
    printf("   Joined with ' | ': %s\n\n", string_cstr(joined));
    
    printf("4. Substring extraction:\n");
    String *sub = string_substring(greeting, 0, 5);
    printf("   First 5 chars: %s\n\n", string_cstr(sub));
    
    printf("5. String comparison:\n");
    String *str1 = string_new("test");
    String *str2 = string_new("test");
    String *str3 = string_new("different");
    printf("   'test' == 'test': %s\n", 
           string_equals(str1, str2) ? "true" : "false");
    printf("   'test' == 'different': %s\n\n",
           string_equals(str1, str3) ? "true" : "false");
    
    printf("6. String cloning:\n");
    String *clone = string_clone(greeting);
    printf("   Original: %s\n", string_cstr(greeting));
    printf("   Clone: %s\n\n", string_cstr(clone));
    
    string_free(greeting);
    string_free(joined);
    string_free(sub);
    string_free(str1);
    string_free(str2);
    string_free(str3);
    string_free(clone);
    string_array_free(words);
    
    printf("All strings automatically freed!\n");
    
    return 0;
}
