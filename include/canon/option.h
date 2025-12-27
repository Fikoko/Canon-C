#ifndef CANON_OPTION_H
#define CANON_OPTION_H

#include <stdbool.h>
#include <stddef.h>

#define OPTION(T) \
    struct { \
        bool is_some; \
        T value; \
    }

#define SOME(val) { .is_some = true, .value = (val) }
#define NONE { .is_some = false }

#define option_is_some(opt) ((opt).is_some)
#define option_is_none(opt) (!(opt).is_some)
#define option_unwrap(opt) ((opt).value)
#define option_unwrap_or(opt, default_val) ((opt).is_some ? (opt).value : (default_val))

typedef OPTION(void*) OptionPtr;

OptionPtr option_ptr_some(void *value);
OptionPtr option_ptr_none(void);

#endif
