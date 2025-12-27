#ifndef CANON_RESULT_H
#define CANON_RESULT_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    RESULT_OK,
    RESULT_ERR
} ResultStatus;

#define RESULT(T, E) \
    struct { \
        ResultStatus status; \
        union { \
            T ok; \
            E err; \
        } data; \
    }

#define OK(val) { .status = RESULT_OK, .data = { .ok = (val) } }
#define ERR(val) { .status = RESULT_ERR, .data = { .err = (val) } }

#define result_is_ok(res) ((res).status == RESULT_OK)
#define result_is_err(res) ((res).status == RESULT_ERR)
#define result_unwrap(res) ((res).data.ok)
#define result_unwrap_err(res) ((res).data.err)
#define result_unwrap_or(res, default_val) (result_is_ok(res) ? (res).data.ok : (default_val))

typedef RESULT(void*, const char*) ResultPtr;

ResultPtr result_ptr_ok(void *value);
ResultPtr result_ptr_err(const char *error);

#endif
