#include "canon/result.h"

ResultPtr result_ptr_ok(void *value) {
    ResultPtr result = { .status = RESULT_OK, .data = { .ok = value } };
    return result;
}

ResultPtr result_ptr_err(const char *error) {
    ResultPtr result = { .status = RESULT_ERR, .data = { .err = error } };
    return result;
}
