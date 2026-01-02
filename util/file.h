#ifndef CANON_C_UTIL_FILE_H
#define CANON_C_UTIL_FILE_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "semantics/option.h"
#include "semantics/result.h"
#include "semantics/Error.h"
#include "core/arena.h"

/*
    file.h — Safe, explicit file operations

    All functions:
      - Use explicit paths and modes
      - Return Option/Result for failure
      - Prefer arena-backed allocation when reading
*/

CANON_C_DEFINE_OPTION(char*)          // for file contents
CANON_C_DEFINE_RESULT(size_t, Error)  // for write sizes

/* Read entire file into arena (preferred) */
static inline option_charp file_read_all_arena(
    const char* path,
    Arena* arena
)
{
    if (!path || !arena) return option_charp_none();

    FILE* f = fopen(path, "rb");
    if (!f) return option_charp_none();

    SCOPE_DEFER { fclose(f); };

    if (fseek(f, 0, SEEK_END) != 0) return option_charp_none();
    long len = ftell(f);
    if (len < 0) return option_charp_none();
    if (fseek(f, 0, SEEK_SET) != 0) return option_charp_none();

    size_t size = (size_t)len + 1;  // +1 for null terminator
    char* buf = arena_alloc(arena, size);
    if (!buf) return option_charp_none();

    if (fread(buf, 1, len, f) != (size_t)len) return option_charp_none();

    buf[len] = '\0';
    return option_charp_some(buf);
}

/* Read entire file into heap (fallback) */
static inline option_charp file_read_all(const char* path)
{
    Arena scratch;
    uint8_t temp[4096];
    arena_init(&scratch, temp, sizeof(temp));
    SCOPE_DEFER { arena_reset(&scratch); };

    option_charp res = file_read_all_arena(path, &scratch);
    if (option_charp_is_none(res)) return option_charp_none();

    return str_alloc_copy(option_charp_unwrap(res));
}

/* Write string to file */
static inline result_size_t_Error file_write_all(
    const char* path,
    const char* content
)
{
    if (!path || !content) return RESULT_ERR(size_t, ERR_INVALID_ARG);

    FILE* f = fopen(path, "wb");
    if (!f) return RESULT_ERR(size_t, ERR_IO_FAILED);

    SCOPE_DEFER { fclose(f); };

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, f);
    if (written != len) return RESULT_ERR(size_t, ERR_IO_FAILED);

    return RESULT_OK(size_t, written);
}

#endif /* CANON_C_UTIL_FILE_H */
