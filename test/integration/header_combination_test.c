/**
 * @file header_combination_test.c
 * @brief Regression guard: canon public headers must be combinable.
 *
 * Several headers instantiate the shared `result_bool_Error` /
 * `option_*` types. Each such instantiation declares a struct tag, and a
 * second declaration of the same tag in one translation unit is a
 * redefinition error — so every instantiation site must participate in the
 * `CANON_RESULT_BOOL_ERROR_DEFINED` guard protocol.
 *
 * Before the fix accompanying this test, ten public header pairs failed to
 * compile together (e.g. fold.h + priority_queue.h, priority_queue.h +
 * log.h). No test combined headers, so nothing caught it. This file exists
 * to make that class of breakage a test failure rather than a user's
 * problem.
 *
 * Deliberately includes the widest combination that is expected to work.
 * Template headers requiring pre-definition of configuration macros
 * (hashmap's HASHMAP_KEY_TYPE, bitset's option_usize dependency) are
 * excluded by design, not by oversight.
 */
#include "algo/fold/fold.h"
#include "algo/fold/fold_decl.h"
#include "data/priority_queue.h"
#include "data/range.h"
#include "data/vec/vec.h"
#include "data/deque/deque.h"
#include "data/stringbuf.h"
#include "semantics/option/option.h"
#include "semantics/result/result.h"
#include "util/parse.h"
#include "util/file.h"
#include "util/log/log.h"
#include "util/str/str.h"

#include <stdio.h>

int main(void) {
    /* The shared instantiation is usable under BOTH spellings: the mangled
       canonical name and log.h's documented alias. */
    result__Bool_Error a = result__Bool_Error_ok(true);
    result_bool_Error   b = result_bool_Error_ok(true);

    if (!result__Bool_Error_is_ok(a)) { printf("FAIL: canonical spelling\n"); return 1; }
    if (!result_bool_Error_is_ok(b))  { printf("FAIL: log.h alias\n");       return 1; }

    /* Both spellings must denote the SAME type, not two parallel ones. */
    a = b;
    if (!result__Bool_Error_is_ok(a)) { printf("FAIL: alias is a distinct type\n"); return 1; }

    printf("OK  header_combination_test  (all assertions passed)\n");
    return 0;
}
