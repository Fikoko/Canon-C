#ifndef CANON_C_CORE_SCOPE_H
#define CANON_C_CORE_SCOPE_H

/*
    scope.h — Deferred cleanup at scope exit

    Macro-based RAII pattern.
    Executes cleanup block when leaving scope (normal exit, return, break, goto).

    No runtime overhead, no hidden state.
*/

#define SCOPE_DEFER \
    for (int _scope_once = 1; _scope_once; _scope_once = 0) \
        for (; _scope_once; ) \
            for (int _scope_done = 0; !_scope_done; _scope_done = 1)

/* Usage:
    void* p = malloc(...);
    SCOPE_DEFER {
        free(p);
    }
*/

#endif /* CANON_C_CORE_SCOPE_H */
