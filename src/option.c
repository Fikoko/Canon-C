#include "canon/option.h"

OptionPtr option_ptr_some(void *value) {
    OptionPtr opt = { .is_some = true, .value = value };
    return opt;
}

OptionPtr option_ptr_none(void) {
    OptionPtr opt = { .is_some = false, .value = NULL };
    return opt;
}
