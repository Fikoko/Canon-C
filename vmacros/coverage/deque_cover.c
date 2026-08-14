/***************************************************************************
 * Copyright (C) 2026 Eclipse Canon-C contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * AI Disclosure: This file was largely AI-generated.
 * The AI-generated portions may be considered public domain (CC0-1.0)
 * and not subject to the project's licence. The human contributor has
 * reviewed and verified that the code is correct.
 *
 * SPDX-License-Identifier: MIT AND CC0-1.0
 **************************************************************************/

/* vmacros/coverage/deque_cover.c                                  [MCDC-011]
 * ============================================================================
 * MC/DC cover translation unit for the deque macro module.
 *
 * Shape: **B — CONFIRMED at CI #1234, re-confirmed #1237/#1238.** THIS FILE
 * is what earned the upgrade: the attribution check is the evidence, not the
 * WP run, which does not test attribution at all. Expected fingerprint, per
 * vec's precedent:
 *   - deque_test.c owns all test-measured outcomes;
 *   - deque_impl.h shows FUNCTIONS BUT NO CONDITIONS (the Shape-A-drift
 *     tripwire — conditions appearing there would mean a body got written
 *     out as real source lines and the module is not Shape B after all);
 *   - the generated conditions attribute to deque_cover.c itself (vec's
 *     third attribution variant: direct instantiation).
 * Record the outcome in MCDC-011 and only then update the status table.
 *
 * WHY THIS FILE EXISTS: same reason as option_cover.c / vec_cover.c — the
 * condition sites live in IMPL_DEQUE_* macro BODIES and are stamped to the
 * expansion site. test/data/deque_test.c's instantiation stamps them to a
 * path the coverage job's test-glob filter removes. This TU re-instantiates
 * DEFINE_DEQUE(int) outside test/ so the identical conditions survive.
 *
 * NOT A CTEST. Built only under -DENABLE_COVERAGE_TUS=ON, invoked directly
 * by the coverage job, never registered with add_test(), never globbed.
 *
 * COMPILE FLAGS must match the WP run and the coverage build:
 *   -DCANON_NO_REQUIRE -DNDEBUG   --coverage -fcondition-coverage
 *
 * Under that flag pair the measured surface changes vs. a debug build:
 *   - require_msg conditions VANISH (macro -> ((void)0)). deque has NO
 *     ensure_msg sites at all.
 *   - init's four guards, both push_*_unchecked triples, swap's pair, and
 *     — the deque-specific one — **peek_front/peek_back's d and out
 *     guards** all disappear. See F1 in deque_verify.h: peek_* is guarded
 *     by require_msg where pop_* checks at runtime, so peek_*(NULL, ...)
 *     is UB under these flags while pop_*(NULL, ...) stays well-defined.
 *     The NULL legs of peek_front / peek_back / peek_*_option / swap /
 *     the unchecked pushes are therefore deliberately NOT exercised here.
 *
 * DIFFERENCE FROM vec_cover.c worth noting at review: vec_cover.c calls
 * vec_int_init(NULL, 0) as a legal spec'd input. deque's init requires
 * buffer != NULL AND capacity > 0, so the analogous call would violate the
 * (compiled-out) precondition. The buffer==NULL / capacity==0 state is
 * reached here the spec'd way instead — via deque_int_empty(). Likewise no
 * over-capacity init call: init has zero conditions under these flags, so
 * nothing is measured by violating its contract.
 *
 * ── PREDICTED-UNCOVERABLE CONDITIONS (write-down before the first run) ─────
 *
 *   **NONE. Predicted 100%, zero justification rows.**
 *
 * This is deliberately the strongest prediction any cover TU in the project
 * has made, and it is meant to be falsifiable in one glance. The reasoning:
 * vec's three uncoverables were U1/U2 (`!checked_mul` true-outcome,
 * guard-redundancy-infeasible) and U3 (`!buf` heap-OOM, environmental).
 * All three came from ALLOCATION. deque allocates nothing — the buffer is
 * caller-owned, there is no alloc/arena_alloc/free family, and checked.h is
 * not even in the include closure. Every remaining condition is a NULL
 * test, a size/capacity comparison, or a ring-index ternary, and each is
 * driven to both outcomes below.
 *
 * If any condition comes back uncovered, the disposition is NOT to add a
 * justification row — it is to find the input that drives it, because the
 * argument above says one exists. Only a genuine infeasibility argument
 * should ever turn this into a justified row.
 *
 * ── RUN-1 RESULT (CI #1231): 79/82, three outcomes uncovered ───────────────
 * The prediction of 100% was WRONG on the measurement and RIGHT on the
 * disposition: all three misses were state-tracking errors in THIS FILE, not
 * properties of deque, and every one was fixed by finding the driving input
 * exactly as the paragraph above prescribes. Zero justification rows added.
 *   1-2. peek_back / pop_back `tail == 0` FALSE leg. Every tail==0 evaluation
 *        in the original file was TRUE. With capacity 4 and tail at 3, the
 *        push_back at the "tail 0 -> 1" comment wraps tail straight back to
 *        0, so the two lines that claimed to drive the FALSE leg drove the
 *        TRUE leg a second time. Fixed in the b3 block, where the state is
 *        controlled: push through the BACK to move tail off zero first.
 *        The stale comments are corrected rather than deleted, because the
 *        wrong claim is the interesting part.
 *   3.   `while (!deque_int_is_full(&d))` guard TRUE outcome. d was already
 *        4/4 at that point, so the loop body never ran. Fixed with one
 *        pop_front ahead of the loop.
 * None of the three fixes adds a condition site, so the denominator stays at
 * 41/82 and the 100% prediction remains testable on run 2 unchanged.
 * REVISED PREDICTION FOR RUN 2: 82/82.
 *
 * ── RUN-2 RESULT (CI #1234): 82/82, 100.00%. CONFIRMED. ───────────────────
 * Denominator exactly as pre-counted (41 conditions / 82 outcomes), zero
 * justification rows, zero uncoverable conditions. The original prediction
 * therefore stands as written: deque allocates nothing, so it has none of
 * the allocation-derived uncoverables that give vec its three. Run 1's three
 * misses were this file's bookkeeping, not deque's, and saying so before
 * looking was the point.
 *
 * MCDC-011 is closeable on this evidence, together with the attribution
 * check in the coverage job: deque_impl.h reported "No conditions" under
 * BOTH deque_test.c and deque_cover.c, so the Shape-A-drift tripwire did not
 * fire and deque is Shape B **confirmed** — update the status table from
 * "(provisional)". Note this is the coverage stream's evidence alone; the WP
 * run neither tests nor supports the shape claim. 
 *
 * ── PREDICTED CONDITION COUNT (also write-down-before) ─────────────────────
 * **39 generated conditions / 78 outcomes**, plus 2 scaffolding conditions
 * (4 outcomes) in this TU's own fill and wrap-around loops.
 * **MCDC-011 denominator: 41 conditions / 82 outcomes.**
 *
 * These are not estimates — they were counted statically off the
 * preprocessed expansion under this exact flag pair, per function:
 *   len 1, capacity 1, remaining 1                          =  3
 *   is_empty 2, is_full 2                                   =  4
 *   push_front 4, push_back 3                               =  7
 *   try_push_front 4, try_push_back 3                       =  7
 *   push_front_unchecked 1, push_back_unchecked 0           =  1
 *   pop_front 4, pop_back 5                                 =  9
 *   pop_front_option 1, pop_back_option 1                   =  2
 *   peek_front 1, peek_back 2                               =  3
 *   peek_front_option 1, peek_back_option 1                 =  2
 *   clear 1                                                 =  1
 *   swap 0, init 0, empty 0, lifetime_open_ 0               =  0
 *                                                             ---
 *                                                              39
 * A different generated total means the flag pair is not what this banner
 * assumes — check that require_msg really compiled out before chasing it.
 * (Note how much of the surface is invisible in a debug build: init, swap
 * and both unchecked pushes contribute 1 condition between them here,
 * against 10 require_msg guards with -DCANON_NO_REQUIRE absent.)
 *
 * Representative instantiation: int — same rationale as the WP driver.
 * ============================================================================ */

/* Handler symbol for the link (standalone binary), same as the other cover
 * TUs. No deque path reaches it under CANON_NO_REQUIRE — deque has zero
 * direct CANON_INVOKE_HANDLER_ calls — but contract.h's extern still needs
 * a home. */
#define CANON_CONTRACT_IMPL

#include "semantics/option/option.h"
#include "data/deque/deque.h"

CANON_OPTION(int)
DEFINE_DEQUE(static inline, int)

/* Sink so the compiler cannot dead-strip under -O / NDEBUG. */
static volatile int   g_sink;
static volatile usize g_usink;

/* NULL-safe query battery. Safe to call with d == NULL: every function here
 * tests d at runtime. peek_* is NOT in this list, by F1. */
static void observe(const deque_int* d)
{
    g_usink = deque_int_len(d);
    g_usink = deque_int_capacity(d);
    g_usink = deque_int_remaining(d);
    g_sink  = (int)deque_int_is_empty(d);
    g_sink  = (int)deque_int_is_full(d);
}

int main(void)
{
    int buf[4];
    int out = 0;
    result__Bool_Error r;

    /* ── constructors ──────────────────────────────────────────────────── */
    deque_int d = deque_int_empty();
    deque_int e = deque_int_empty();          /* buffer==NULL, capacity==0  */
    deque_int_init(&d, buf, 4);               /* spec-satisfying init       */

    /* ── queries: NULL leg, live-empty leg, and (later) full leg ───────── */
    observe(NULL);                            /* !d TRUE  / d? FALSE        */
    observe(&d);                              /* !d FALSE / d? TRUE,
                                                 size==0 TRUE, size>=cap F  */
    observe(&e);                              /* capacity==0: size>=cap T   */

    /* ── push_front: all four conditions ───────────────────────────────── */
    r = deque_int_push_front(NULL, 1);   (void)r;  /* !d TRUE               */
    r = deque_int_push_front(&e, 1);     (void)r;  /* !buffer TRUE          */
    r = deque_int_push_front(&d, 10);    (void)r;  /* ok; head==0 TRUE      */
    r = deque_int_push_front(&d, 11);    (void)r;  /* ok; head==0 FALSE     */

    /* ── pop_back with tail==0 TRUE ────────────────────────────────────────
       Only push_front has run, so tail is still 0 while size > 0. This is
       the ring-specific leg that a shifting buffer could never produce. */
    r = deque_int_pop_back(&d, &out);    (void)r;  /* tail==0 TRUE          */

    /* ── push_back: three conditions ───────────────────────────────────── */
    r = deque_int_push_back(NULL, 1);    (void)r;  /* !d TRUE               */
    r = deque_int_push_back(&e, 1);      (void)r;  /* !buffer TRUE          */
    r = deque_int_push_back(&d, 20);     (void)r;  /* ok — tail 0 -> 1      */

    /* ── pop_back / peek_back again ────────────────────────────────────────
       RUN-1 CORRECTION: these two lines previously claimed to drive the
       tail==0 FALSE leg. They do not. With capacity 4 and tail at 3, the
       push_back above wraps tail back to 0, so every tail==0 evaluation in
       this function is TRUE. The FALSE leg is driven in the b3 block below,
       on a deque whose state is controlled for exactly that purpose. */
    g_sink = (int)deque_int_peek_back(&d, &out);
    r = deque_int_pop_back(&d, &out);    (void)r;

    /* ── try_push family ───────────────────────────────────────────────── */
    g_sink = (int)deque_int_try_push_front(NULL, 1);  /* !d TRUE            */
    g_sink = (int)deque_int_try_push_front(&e, 1);    /* !buffer TRUE       */
    g_sink = (int)deque_int_try_push_front(&d, 30);   /* ok; head==0 ?      */
    g_sink = (int)deque_int_try_push_front(&d, 31);   /* ok; head other leg */
    g_sink = (int)deque_int_try_push_back(NULL, 1);   /* !d TRUE            */
    g_sink = (int)deque_int_try_push_back(&e, 1);     /* !buffer TRUE       */
    g_sink = (int)deque_int_try_push_back(&d, 40);    /* ok                 */

    /* ── fill to capacity, then the size>=capacity TRUE legs ───────────────
       Scaffolding condition #1.
       RUN-1 CORRECTION: d was already at 4/4 here, so the loop body never
       executed and the guard's TRUE outcome went uncovered. One pop_front
       makes room so the loop is entered at least once. */
    r = deque_int_pop_front(&d, &out);   (void)r;  /* make room: 4/4 -> 3/4 */
    while (!deque_int_is_full(&d)) { (void)deque_int_try_push_back(&d, 0); }
    observe(&d);                                   /* is_full TRUE leg      */
    r = deque_int_push_front(&d, 99);    (void)r;  /* size>=cap TRUE        */
    r = deque_int_push_back(&d, 99);     (void)r;  /* size>=cap TRUE        */
    g_sink = (int)deque_int_try_push_front(&d, 99);/* size>=cap TRUE        */
    g_sink = (int)deque_int_try_push_back(&d, 99); /* size>=cap TRUE        */

    /* ── pop_front: all four conditions ────────────────────────────────── */
    r = deque_int_pop_front(NULL, &out); (void)r;  /* !d TRUE               */
    r = deque_int_pop_front(&d, NULL);   (void)r;  /* !out TRUE             */
    r = deque_int_pop_front(&e, &out);   (void)r;  /* !buffer TRUE          */
    r = deque_int_pop_front(&d, &out);   (void)r;  /* ok, size>0            */

    /* ── pop_back: the !d / !out / !buffer legs ────────────────────────── */
    r = deque_int_pop_back(NULL, &out);  (void)r;  /* !d TRUE               */
    r = deque_int_pop_back(&d, NULL);    (void)r;  /* !out TRUE             */
    r = deque_int_pop_back(&e, &out);    (void)r;  /* !buffer TRUE          */

    /* ── the size==0 legs on a LIVE deque (buffer non-NULL, empty) ──────── */
    {
        int b2[2];
        deque_int t = deque_int_empty();
        deque_int_init(&t, b2, 2);

        r = deque_int_pop_front(&t, &out);  (void)r;   /* size==0 TRUE      */
        r = deque_int_pop_back(&t, &out);   (void)r;   /* size==0 TRUE      */
        g_sink = (int)deque_int_peek_front(&t, &out);  /* size==0 TRUE      */
        g_sink = (int)deque_int_peek_back(&t, &out);   /* size==0 TRUE      */

        /* option wrappers, none leg */
        g_sink = (int)option_int_is_some(deque_int_pop_front_option(&t));
        g_sink = (int)option_int_is_some(deque_int_pop_back_option(&t));
        g_sink = (int)option_int_is_some(deque_int_peek_front_option(&t));
        g_sink = (int)option_int_is_some(deque_int_peek_back_option(&t));

        /* populate, then the some legs and the size>0 peek legs */
        (void)deque_int_try_push_back(&t, 1);
        (void)deque_int_try_push_back(&t, 2);
        g_sink = (int)deque_int_peek_front(&t, &out);  /* size==0 FALSE     */
        g_sink = (int)deque_int_peek_back(&t, &out);   /* size==0 FALSE     */
        g_sink = (int)option_int_is_some(deque_int_peek_front_option(&t));
        g_sink = (int)option_int_is_some(deque_int_peek_back_option(&t));
        g_sink = (int)option_int_is_some(deque_int_pop_front_option(&t));
        g_sink = (int)option_int_is_some(deque_int_pop_back_option(&t));
    }

    /* ── option wrappers: the NULL-deque none legs ─────────────────────────
       pop_*_option delegate to pop_*, which are NULL-safe. peek_*_option
       are NOT (F1) and are therefore absent from this group. */
    g_sink = (int)option_int_is_some(deque_int_pop_front_option(NULL));
    g_sink = (int)option_int_is_some(deque_int_pop_back_option(NULL));
    g_sink = (int)option_int_is_some(deque_int_pop_front_option(&e));
    g_sink = (int)option_int_is_some(deque_int_pop_back_option(&e));

    /* ── peek_back's tail==0 TRUE leg on a live deque ──────────────────────
       Reached by pushing only through the FRONT, which leaves tail at 0. */
    {
        int b3[3];
        deque_int t = deque_int_empty();
        deque_int_init(&t, b3, 3);
        (void)deque_int_try_push_front(&t, 7);         /* head 0 -> 2, tail 0 */
        g_sink = (int)deque_int_peek_back(&t, &out);   /* tail==0 TRUE      */
        g_sink = (int)option_int_is_some(deque_int_peek_back_option(&t));

        /* RUN-1 CORRECTION: advance tail off zero, then re-drive both
           ring-index ternaries on their FALSE leg. Pushing through the BACK
           is what moves tail; nothing earlier in this TU left tail non-zero
           at a point where peek_back / pop_back were called. */
        (void)deque_int_try_push_back(&t, 8);          /* tail 0 -> 1       */
        g_sink = (int)deque_int_peek_back(&t, &out);   /* tail==0 FALSE     */
        r = deque_int_pop_back(&t, &out);   (void)r;   /* tail==0 FALSE     */
        r = deque_int_pop_back(&t, &out);   (void)r;   /* tail==0 TRUE      */
    }

    /* ── unchecked pushes (preconditions satisfied; NULL legs are UB) ──────
       push_front_unchecked carries the only condition of the two: its
       head==0 ternary. Both legs driven from a fresh deque. */
    {
        int b4[4];
        deque_int t = deque_int_empty();
        deque_int_init(&t, b4, 4);
        deque_int_push_front_unchecked(&t, 1);   /* head==0 TRUE  -> head 3 */
        deque_int_push_front_unchecked(&t, 2);   /* head==0 FALSE -> head 2 */
        deque_int_push_back_unchecked(&t, 3);    /* no conditions           */
        observe(&t);
    }

    /* ── clear ─────────────────────────────────────────────────────────── */
    deque_int_clear(NULL);                       /* !d TRUE                 */
    deque_int_clear(&d);                         /* !d FALSE                */
    observe(&d);

    /* ── swap (NULL legs are UB under CANON_NO_REQUIRE — not exercised) ── */
    {
        int ba[2], bb[2];
        deque_int a = deque_int_empty();
        deque_int b = deque_int_empty();
        deque_int_init(&a, ba, 2);
        deque_int_init(&b, bb, 2);
        (void)deque_int_try_push_back(&a, 1);
        deque_int_swap(&a, &b);
        observe(&a); observe(&b);
    }

    /* ── wrap-around exercise ──────────────────────────────────────────────
       Not a new condition site, but it drives the modular index arithmetic
       through the head/tail wrap that the straight-line pushes above never
       reach — the MC/DC-side companion to the WP driver's ring predicate.
       Scaffolding condition #2. */
    {
        int b2[2];
        deque_int t = deque_int_empty();
        deque_int_init(&t, b2, 2);
        for (int i = 0; i < 6; i++) {
            (void)deque_int_try_push_back(&t, i);
            r = deque_int_pop_front(&t, &out);   (void)r;
        }
        observe(&t);
    }

    (void)g_sink; (void)g_usink;
    return 0;
}
