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

#ifndef CANON_DATA_BITSET_H
#define CANON_DATA_BITSET_H

#include "core/primitives/types.h"
#include "core/primitives/limits.h"
#include "core/primitives/bits.h"
#include "core/primitives/contract.h"
#include "core/memory.h"
#include "core/slice.h"
#include "core/ownership.h"
#include "semantics/option/option.h"
#include "semantics/borrow.h"

#ifdef CANON_LIFETIME_DEBUG
    #include <stdint.h>                    /* uintptr_t */
    #include "core/primitives/lifetime.h"  /* region_id_t, lifetime_t */
#endif

/**
 * @brief Instantiate option_usize exactly once across all translation units
 *
 * bitset_find_first_option / find_next_option / find_last_option return
 * option_usize. Until 2026-08-22 this header did NOT instantiate it and
 * required every includer to do so first — test/data/bitset_test.c carried
 * the instruction in a comment, and bitset.h was the only header in the tree
 * that made a caller pre-instantiate a generic. That asymmetry was found by
 * VERIFY-020: frama-c, handed the header directly, aborted in the PARSER at
 * bitset_find_first_option with "syntax error", because it had no includer to
 * do the instantiating.
 *
 * The guard convention is vec_impl.h's, verbatim (see the
 * CANON_RESULT_BOOL_ERROR_DEFINED block there): instantiate behind a
 * type-specific guard so that a caller wanting a DIFFERENT option_usize —
 * notably vmacros/vdrivers/bitset_verify.h, which needs a CONTRACTED one —
 * defines the guard first and interposes its own. Callers that already
 * pre-instantiate with a bare CANON_OPTION(usize) must now either drop that
 * line or define the guard; see the note in bitset_test.c.
 *
 * PLACEMENT: this block sits AFTER every #include in the file, deliberately.
 * MISRA C:2012 rule 20.1 requires an #include to be preceded only by
 * preprocessor directives or comments, and CANON_OPTION expands to
 * DECLARATIONS. Placing it above the CANON_LIFETIME_DEBUG include block put
 * two #includes behind a declaration and raised the advisory MISRA count from
 * 53 to 55 (data/bitset.h:56-57, rule 20.1) on its first scan. Keep it last.
 */
#ifndef CANON_OPTION_USIZE_DEFINED
    #define CANON_OPTION_USIZE_DEFINED
    /* cppcheck-suppress misra-c2012-19.2 ; MISRA-DEV-014 */
    CANON_OPTION(usize)
#endif

/**
 * @file data/bitset.h
 * @brief Fixed-capacity bitset with O(1) set/clear/test and O(n/64) bulk ops
 *
 * A Bitset is a compact array of bits backed by a caller-provided u64 buffer.
 * Capacity is fixed at initialization. No allocation inside any function.
 *
 * Core ideas:
 * ────────────────────────────────────────────────────────────────────────────
 * - Capacity is fixed — no growth, no reallocation
 * - Caller owns the backing buffer (stack, arena, static)
 * - All bit operations are O(1); bulk ops (count, find, and/or/xor) are O(n/64)
 * - Backed by u64 words — 64 bits per word, cache-friendly
 * - Bits beyond capacity are always kept zero (padding invariant)
 * - bytes_t view available via bitset_as_bytes() for I/O and serialization
 * - Tracked borrowed_bytes view available via bitset_as_borrowed_bytes()
 *   when borrows need to participate in the lifetime substrate
 *
 * NULL contract (uniform across all functions):
 * ────────────────────────────────────────────────────────────────────────────
 * - A NULL Bitset* is always a silent no-op for void functions
 * - Query functions return 0, false, or BITSET_NPOS for NULL input
 * - option_usize-returning find functions return None for NULL input
 * - Out-of-bounds index on a valid (non-NULL) Bitset* is a contract violation
 *   and fires require_msg — it is a programming error, not a recoverable condition
 *
 * Typed find functions (preferred):
 * ────────────────────────────────────────────────────────────────────────────
 * bitset_find_first_option / bitset_find_next_option / bitset_find_last_option
 * return option_usize instead of the BITSET_NPOS sentinel. Use these in
 * preference to the raw variants in new code.
 *
 * option_usize is instantiated by this header (guard: CANON_OPTION_USIZE_DEFINED).
 * Callers need do nothing. To supply your own — e.g. a contracted instantiation
 * for verification, as vmacros/vdrivers/bitset_verify.h does — define that
 * guard and emit it before including.
 *
 * The raw sentinel-returning variants (bitset_find_first, bitset_find_next,
 * bitset_find_last) are kept for BITSET_FOR_EACH and performance-critical
 * tight loops where the option wrapper overhead is undesirable.
 *
 * Lifetime tracking (define CANON_LIFETIME_DEBUG before including):
 * ────────────────────────────────────────────────────────────────────────────
 *   - Embeds a lifetime_t lt field (id + open) on the Bitset.
 *   - bitset_init() opens a fresh lifetime; the ID is derived from a per-TU
 *     monotonic counter XOR'd with &bs. Borrows constructed via
 *     bitset_as_borrowed_bytes() capture this ID.
 *   - bitset_close() CLOSES the lifetime (lt.open = false). Any
 *     borrowed_bytes captured prior to close fires require_msg via
 *     lifetime_assert_valid() on the next read.
 *   - Mutating operations (set / clear / toggle / assign / set_all /
 *     clear_all / not / and / or / xor) do NOT restamp. Bitset has fixed
 *     capacity, never reallocates, and the backing word array's address
 *     is stable for the Bitset's whole lifetime. A borrowed_bytes view
 *     captured against the words still points at valid memory after any
 *     mutation; it just reads the new bit values. This matches the
 *     substrate's documented honesty boundary used by vec, deque, and
 *     the convenience containers: lifetime tracking catches
 *     use-after-relocation and use-after-close, not use-of-mutated-value.
 *   Zero cost in release builds — struct layout is identical without the flag.
 *
 * Dependency rule:
 * ────────────────────────────────────────────────────────────────────────────
 * data/bitset.h is in data/. It may depend on core/ and semantics/.
 * No other data/ headers may be included here.
 *
 * Thread-safety:
 * ────────────────────────────────────────────────────────────────────────────
 * NOT thread-safe. Caller must synchronize if shared across threads.
 *
 * Portability:
 * ────────────────────────────────────────────────────────────────────────────
 * - Requires C99 or later
 * - Assumes u64 is 64 bits (guaranteed by core/primitives/types.h)
 * - No platform-specific code
 *
 * Performance:
 * ────────────────────────────────────────────────────────────────────────────
 * - set / clear / toggle / test:  O(1)
 * - count (popcount all bits):    O(n/64)
 * - find_first / find_next:       O(n/64) worst case
 * - and / or / xor / not:         O(n/64)
 * - clear_all / set_all:          O(n/64)
 *
 * Buffer sizing:
 * ────────────────────────────────────────────────────────────────────────────
 * Use BITSET_WORD_COUNT(n) to compute the number of u64 words needed for n bits.
 *
 * Quick start:
 * ```c
 * #include "data/bitset.h"
 *
 * // option_usize comes with the header — no pre-instantiation needed.
 * // (This example previously showed a CANON_OPTION(usize) line AFTER the
 * //  include, which could never have worked in either direction.)
 *
 * // Stack-backed bitset for 200 bits
 * u64 words[BITSET_WORD_COUNT(200)];
 * Bitset bs;
 * bitset_init(&bs, words, 200);
 *
 * bitset_set(&bs, 42);
 * bitset_set(&bs, 199);
 * bool v = bitset_test(&bs, 42);      // true
 * usize n = bitset_count(&bs);        // 2
 *
 * // Preferred — typed option variants
 * option_usize f = bitset_find_first_option(&bs);   // Some(42)
 * if (option_usize_is_some(f)) { ... }
 *
 * // Legacy sentinel variants (used by BITSET_FOR_EACH)
 * usize raw = bitset_find_first(&bs);               // 42
 *
 * // Arena-backed
 * u64* buf = arena_alloc(&arena, BITSET_WORD_COUNT(1024) * sizeof(u64));
 * Bitset bs2;
 * bitset_init(&bs2, buf, 1024);
 *
 * // Tracked borrow (CANON_LIFETIME_DEBUG)
 * borrowed_bytes view = bitset_as_borrowed_bytes(&bs);
 * // ... use borrowed_bytes_get(&view) ...
 * bitset_close(&bs);   // invalidates view
 * ```
 *
 * @sa core/primitives/bits.h    — bits_popcount, bits_ctz, bits_clz used internally
 * @sa core/slice.h              — bytes_t used by bitset_as_bytes()
 * @sa core/ownership.h          — borrowed() annotation
 * @sa semantics/option/option.h — option_usize for typed find results
 * @sa semantics/borrow.h        — borrowed_bytes used by bitset_as_borrowed_bytes()
 */

/* ════════════════════════════════════════════════════════════════════════════
   Constants and sizing macros
   ════════════════════════════════════════════════════════════════════════════ */

/** @brief Bits per backing word */
#define BITSET_BITS_PER_WORD ((usize)64)

/**
 * @brief Number of u64 words required to hold n bits
 *
 * Example: BITSET_WORD_COUNT(64) == 1, BITSET_WORD_COUNT(65) == 2
 */
#define BITSET_WORD_COUNT(n) \
    (((usize)(n) + BITSET_BITS_PER_WORD - 1u) / BITSET_BITS_PER_WORD)

/** @brief Sentinel returned by bitset_find_first / bitset_find_next / bitset_find_last when no bit is set */
#define BITSET_NPOS CANON_USIZE_MAX

/* ════════════════════════════════════════════════════════════════════════════
   Bitset struct
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Fixed-capacity bitset
 *
 * Do not access fields directly — use the provided functions.
 *
 * Invariant: bits at indices [capacity, word_count*64) are always zero.
 *
 * Memory layout:
 * - sizeof(Bitset) = sizeof(u64*) + 2*sizeof(usize)
 *   [+ sizeof(lifetime_t) under CANON_LIFETIME_DEBUG]
 */
typedef struct {
    u64*  words;      ///< Backing word array (caller-owned)
    usize capacity;   ///< Number of usable bits
    usize word_count; ///< Number of u64 words in backing array
#ifdef CANON_LIFETIME_DEBUG
    lifetime_t lt;    ///< [debug] Lifetime token: id + open
#endif
} Bitset;

/* ════════════════════════════════════════════════════════════════════════════
   ACSL logic layer                                              [VERIFY-020]
   ════════════════════════════════════════════════════════════════════════════
   Shape follows vmacros/vdrivers/deque_verify.h: a read predicate, a write
   predicate that strengthens it, and the maintainable structural invariant
   kept SEPARATE — deque calls its one `ring`, bitset's is `bitset_pad`.
   Keeping it separate lets the functions that must RESTORE it (set_all, not)
   say so explicitly, without forcing every function that merely preserves it
   to re-state it as a preconditon it does not need.

   ON bitset_pad VS bitset_pad_meaning
   -----------------------------------
   The invariant's INTENT is "no bit at or above capacity is set". That is
   bitset_pad_meaning, and it is the sentence a reader wants. What the code
   actually establishes is a masking equation on the final word, which is
   bitset_pad. The two are equivalent, but the equivalence is bit-level
   reasoning the SMT backend is not expected to discharge, so ONLY bitset_pad
   appears in contracts.

       bitset_pad(bs) ==> bitset_pad_meaning(bs)

   is therefore a MANUAL-PROOF obligation, recorded in VERIFY-020 rather than
   asserted here by an annotation nothing checks. This is the lifetime.h
   lesson applied ahead of time: two of the eleven superseded copies of
   canon_lifetime_next_id_ carried `assigns \nothing` while writing their
   counter, and the falsehood survived precisely because no verified
   configuration ever translated it.

   MODEL: Typed+Cast, forced by bitset_as_bytes / bitset_as_cbytes, which
   hand bs->words (u64*) to bytes_from / cbytes_from as a byte view. Goal
   names therefore carry the typed_cast_ prefix, as in diag.h and vec.
   ════════════════════════════════════════════════════════════════════════════ */

/* ── E1 / E1a: THE DIVISION HYPOTHESIS, REFUTED ─────────────────────────────
   Two experiments, two multi-hour runs, one negative result. Recorded in
   full because the negative result is the useful part.

   THE HYPOTHESIS
   Patch 1 left eighteen Group-1 residuals, all mentioning bitset_pad or
   bitset_mut. E1 proposed that the common factor was INTEGER DIVISION in
   PROVEN positions -- free in a `requires`, expensive in an `ensures` --
   entering through bitset_view's `word_count == bitset_words_for(capacity)`
   and bitset_pad's `capacity % 64`.

   E1 (1eb5d5c, CI #1254): killed the runner TWICE. Not infrastructure --
   every sibling WP job was green on the same pool and commit. Cause: E1
   replaced `>> (capacity % 64)`, whose exponent is SYNTACTICALLY bounded in
   [0,63], with `>> bitset_rem(bs)`, whose bound follows only by deriving it
   from bitset_sized. Until derived, the term is x >> k with k an unbounded
   symbolic expression -- x / 2^k, exponential in a free variable. Memory
   blow-up, which -wp-timeout cannot bound. The redundant-looking disjunct
   removed as a "simplification" was load-bearing FOR THE PROVER though
   redundant for the LOGIC. That is the one durable finding of this arc, and
   it is why bitset_pad's `% 64` and disjunct below must stay.

   E1a (CI #1255): kept only bitset_sized, reverted bitset_pad. Result: ALL
   EIGHTEEN residuals unchanged, same goal indices. The twelve
   bitset_mut/bitset_view goals did not clear. DIVISION WAS NOT THE COST.

   THREE WRONG GUESSES, AND THE CORRECTION
   Predicate size, then a missing lemma, then division. Each cost a run. The
   error was not any single hypothesis but the method: diagnosing by
   archaeology on straight-line goals instead of measuring.

   THE REFRAME THAT SHOULD HAVE COME FIRST
   Eighteen residuals is not a defect. vec carries 196, arena 91, pool 119,
   deque 67 -- residuals are normal here, and what makes them acceptable is
   that they are documented, name-pinned, and carry manual-proof arguments.
   Eighteen would be the SMALLEST own-residual set in the data/ layer.
   The real question was never "can these reach zero" but "are these
   predicates affordable inside LOOP INVARIANTS", and that is answered by
   writing one, not by theorising about postconditions. See bitset_not.
   ──────────────────────────────────────────────────────────────────────────── */

/*@
  logic integer bitset_word_l(integer i)   = i / 64;
  logic integer bitset_bitpos_l(integer i) = i % 64;

  logic integer bitset_words_for(integer n) = (n + 63) / 64;

  predicate bitset_bit_set{L}(Bitset* bs, integer i) =
      (bs->words[bitset_word_l(i)] & ((u64)1 << bitset_bitpos_l(i))) != 0;

  predicate bitset_view{L}(Bitset* bs) =
      \valid_read(bs)
      && bs->capacity > 0
      && bs->capacity <= CANON_USIZE_MAX - 63
      && bs->word_count == bitset_words_for(bs->capacity)
      && \valid_read(bs->words + (0 .. bs->word_count - 1));

  predicate bitset_mut{L}(Bitset* bs) =
      bitset_view(bs)
      && \valid(bs)
      && \valid(bs->words + (0 .. bs->word_count - 1));

  // DO NOT "SIMPLIFY" THIS. The `% 64` keeps the shift exponent
  // syntactically bounded in [0,63], and the disjunct gives the solver a
  // free case split. E1 removed both as redundant and took the CI runner
  // down twice (CI #1254). Redundant for the logic, load-bearing for the
  // prover.
  predicate bitset_pad{L}(Bitset* bs) =
      bs->capacity % 64 == 0
      || (bs->words[bs->word_count - 1] >> (bs->capacity % 64)) == 0;

  predicate bitset_pad_meaning{L}(Bitset* bs) =
      \forall integer i;
          bs->capacity <= i < bs->word_count * 64 ==> !bitset_bit_set(bs, i);
*/

/* ════════════════════════════════════════════════════════════════════════════
   Internal: lifetime helpers (compiled away in release)
   ════════════════════════════════════════════════════════════════════════════
   When CANON_LIFETIME_DEBUG is enabled, a Bitset exposes a lifetime_t
   that borrows constructed via bitset_as_borrowed_bytes() can capture and
   validate against. The helpers below manage the (id, open) pair across
   the Bitset lifecycle:

     bitset_lifetime_open_(bs)
       Called by bitset_init. Draws a fresh id from the per-TU monotonic
       counter (XOR'd with &bs for cross-TU diversity) and marks the
       lifetime open.

     bitset_lifetime_close_(bs)
       Called by bitset_close. Marks the lifetime closed, which makes any
       subsequent borrow read fire lifetime_assert_valid.

   There is intentionally no restamp helper — Bitset has fixed capacity
   and never relocates its backing buffer. Mutating operations on bits
   do not invalidate borrows on the words array. See file docblock for
   the substrate's honesty boundary.

   In release builds (CANON_LIFETIME_DEBUG undefined) both helpers are
   no-ops — the Bitset struct does not have an lt field and no code
   touches it.
   ════════════════════════════════════════════════════════════════════════════ */

#ifdef CANON_LIFETIME_DEBUG
    /* Per-TU counter used to derive unique lifetime ids.
     *
     * The counter is a `static` inside a `static inline` function, so
     * each translation unit has its own copy and increments are TU-local.
     * Two Bitsets initialized in the same TU get different ids; Bitsets
     * initialized in different TUs get ids from independent counters but
     * mixed with the struct address, making cross-TU collisions
     * vanishingly unlikely. Same pattern as vec / deque / pq / hashmap /
     * dynvec / smallvec / dynstring.
     *
     * Unlike the convenience containers that return by value (and where
     * the compiler may reuse stack slots across constructor calls), the
     * Bitset constructor takes the struct by pointer — so the caller's
     * Bitset has a stable address that the counter mixes with cleanly.
     * The counter still adds essential diversity: it guarantees that
     * sequential init / close / init cycles on the same Bitset slot
     * produce different ids, so borrows captured before close cannot
     * silently re-validate against the post-init Bitset.
     *
     * No thread-safety guarantee: if a single TU's bitset_init or
     * bitset_close are invoked concurrently, the counter may race and
     * collide. Same constraint as every other Canon-C container.
     *
     * REGION_ID_STATIC (0) is reserved; the counter starts at 1 and the
     * id derivation never produces 0 defensively.
     */
    /* Delegates to the single shared generator in
     * core/primitives/lifetime.h — see docs/thread-safety.md for the
     * concurrency contract on token generation. */
    static inline region_id_t bitset_lifetime_next_id_(void* bsp) {
        return canon_lifetime_next_id_(bsp);
    }
#endif

/* Contract holds in the VERIFIED configuration (CANON_LIFETIME off), under
   which the body is `(void)bs`. Under CANON_LIFETIME_DEBUG the true clause
   is `assigns bs->lt.id, bs->lt.open` — that configuration is not verified,
   and is not claimed here. */
/*@ requires \valid(bs);
    assigns \nothing;
*/
static inline void bitset_lifetime_open_(Bitset* bs) {
#ifdef CANON_LIFETIME_DEBUG
    bs->lt.id   = bitset_lifetime_next_id_(bs);
    bs->lt.open = true;
#endif
    (void)bs;
}

/* Same configuration note as bitset_lifetime_open_ above. */
/*@ requires \valid(bs);
    assigns \nothing;
*/
static inline void bitset_lifetime_close_(Bitset* bs) {
#ifdef CANON_LIFETIME_DEBUG
    bs->lt.open = false;
#endif
    (void)bs;
}

/* ════════════════════════════════════════════════════════════════════════════
   Internal helpers
   ════════════════════════════════════════════════════════════════════════════ */

/** @brief Word index for bit i */
/*@ assigns \nothing;
    ensures \result == i / 64;
*/
static inline usize bitset_word_of(usize i) { return i / BITSET_BITS_PER_WORD; }

/** @brief Bit mask for bit i within its word */
/* `\result != 0` is load-bearing, not decoration: bitset_set's postcondition
   reduces to (x | m) & m != 0, which is unprovable without it. -wp-rte also
   emits a shift-count obligation here, discharged by i % 64 <= 63. */
/*@ assigns \nothing;
    ensures \result == (u64)((u64)1 << (i % 64));
    ensures \result != 0;
*/
static inline u64 bitset_mask_of(usize i) { return (u64)1 << (i % BITSET_BITS_PER_WORD); }

/**
 * @brief Clears padding bits in the last word to maintain the invariant
 *
 * Called after set_all and bitset_not to ensure bits beyond capacity stay zero.
 */
/* Does NOT require bitset_pad on entry — this is the function that
   establishes it, and set_all / not call it exactly when it is broken.
   `assigns` is an upper bound, so the rem == 0 path writing nothing is
   within the clause. */
/*@ requires bitset_mut(bs);

    assigns bs->words[bs->word_count - 1];

    ensures bitset_pad(bs);
    ensures \forall integer k;
        0 <= k < bs->word_count - 1 ==> bs->words[k] == \old(bs->words[k]);
    ensures bitset_mut(bs);
*/
static inline void bitset_clear_padding(borrowed(Bitset*) bs) {
    require_msg(bs != NULL, "bitset_clear_padding: bs cannot be NULL");
    usize rem = bs->capacity % BITSET_BITS_PER_WORD;
    if (rem != 0u) {
        bs->words[bs->word_count - 1u] &= ((u64)1u << rem) - 1u;
    }
}

/* ════════════════════════════════════════════════════════════════════════════
   Initialization
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Initializes a Bitset with a caller-provided u64 buffer
 *
 * All bits are cleared on initialization.
 *
 * @param bs       Pointer to uninitialized Bitset
 * @param words    Caller-provided buffer of at least BITSET_WORD_COUNT(capacity) u64s
 * @param capacity Number of usable bits (> 0)
 *
 * @pre bs != NULL
 * @pre words != NULL
 * @pre capacity > 0
 * @post All bits cleared, bs ready to use
 *
 * Lifetime (CANON_LIFETIME_DEBUG): opens a fresh lifetime token. The ID
 * is derived from a per-TU counter XOR'd with &bs. Borrows constructed
 * via bitset_as_borrowed_bytes carry this ID.
 *
 * Performance: O(n/64) — clears backing words
 */
/* `capacity <= CANON_USIZE_MAX - 63` is finding F1: BITSET_WORD_COUNT(n) is
   (n + 63) / 64, which wraps above that bound, and the three require_msg
   guards below do not check it. The contract admits the obligation rather
   than pretending it is absent; whether the implementation should also guard
   is a live design question for VERIFY-020, not a cleanup.

   `\separated` is required, not defensive: bs and words are independent
   caller allocations, and mem_zero writes through one while the other is
   being assigned. Without it the frame is unprovable. */
/*@ requires \valid(bs);
    requires capacity > 0;
    requires capacity <= CANON_USIZE_MAX - 63;
    requires \valid(words + (0 .. bitset_words_for(capacity) - 1));
    requires \separated(bs, words + (0 .. bitset_words_for(capacity) - 1));

    assigns bs->words, bs->capacity, bs->word_count,
            words[0 .. bitset_words_for(capacity) - 1];

    ensures bs->words == words;
    ensures bs->capacity == capacity;
    ensures bs->word_count == bitset_words_for(capacity);
    ensures \forall integer k; 0 <= k < bs->word_count ==> bs->words[k] == 0;
    ensures bitset_mut(bs);
    ensures bitset_pad(bs);
*/
static inline void bitset_init(borrowed(Bitset*) bs, borrowed(u64*) words, usize capacity) {
    require_msg(bs       != NULL, "bitset_init: bs cannot be NULL");
    require_msg(words    != NULL, "bitset_init: words cannot be NULL");
    require_msg(capacity  > 0u,    "bitset_init: capacity must be > 0");

    bs->words      = words;
    bs->capacity   = capacity;
    bs->word_count = BITSET_WORD_COUNT(capacity);
    mem_zero(words, bs->word_count * sizeof(u64));
    bitset_lifetime_open_(bs);
}

/**
 * @brief Closes the Bitset's lifetime — invalidates outstanding borrowed views
 *
 * The Bitset's storage (the caller-provided word array) is NOT freed —
 * Bitset never owned it. This function exists purely to signal "I'm done
 * with this Bitset; any borrowed views captured against it should now
 * fail." It is the symmetric counterpart of bitset_init.
 *
 * NULL-safe: bitset_close(NULL) is a no-op.
 *
 * Lifetime (CANON_LIFETIME_DEBUG): CLOSES the lifetime (lt.open = false).
 * Any borrowed_bytes that captured this Bitset's ID will fire require_msg
 * on the next read. The lifetime is reopened only by a fresh bitset_init.
 *
 * In release builds (CANON_LIFETIME_DEBUG undefined): this function
 * compiles to nothing — it is purely a substrate hook. Callers that do
 * not opt into lifetime tracking may still call it freely; it costs
 * nothing.
 *
 * @param bs Bitset to close (NULL-safe)
 *
 * Performance: O(1)
 */
/* Configuration note as bitset_lifetime_close_. Under CANON_LIFETIME_DEBUG
   the true clause is `assigns bs->lt.open`. */
/*@ requires bs == \null || \valid(bs);
    assigns \nothing;
*/
static inline void bitset_close(borrowed(Bitset*) bs) {
    if (!bs) { return; }
    bitset_lifetime_close_(bs);
}

/* ════════════════════════════════════════════════════════════════════════════
   Single-bit operations — O(1)
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Sets bit i (makes it 1)
 *
 * NULL bs is a no-op. Out-of-bounds i on a valid bs is a contract violation.
 *
 * @pre i < bs->capacity (when bs != NULL)
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Backing words array
 * is unchanged in extent and address; existing borrows remain valid.
 *
 * Performance: O(1)
 */
/* `requires bs->words != \null` is finding F2, stated as its own clause and
   NOT folded into bitset_mut. set / clear / toggle / test check only `!bs`,
   while clear_all / set_all / not / count / is_empty / find_* all check
   `!bs || !bs->words`. On a zero-initialized `Bitset b = {0}` the index guard
   below fails and — under -DCANON_NO_REQUIRE, the flag this very WP job uses
   — compiles out, leaving a dereference of words == NULL. Structurally this
   is deque's F1 (peek_* guarded by require_msg where pop_* checks at
   runtime), except that bitset's asymmetry is WITHIN one family rather than
   between two, which makes it more surprising to a caller. Kept as a
   separate clause so that if the implementation grows the runtime check,
   exactly one line moves. */
/*@ requires bs == \null || (bitset_mut(bs) && bitset_pad(bs));
    requires bs == \null || bs->words != \null;
    requires bs == \null || i < bs->capacity;

    behavior null:
      assumes bs == \null;
      assigns \nothing;

    behavior live:
      assumes bs != \null;
      assigns bs->words[bitset_word_l(i)];
      ensures bitset_bit_set(bs, i);
      ensures \forall integer k;
          0 <= k < bs->word_count && k != bitset_word_l(i)
              ==> bs->words[k] == \old(bs->words[k]);
      ensures bitset_pad(bs);
      ensures bitset_mut(bs);

    complete behaviors;
    disjoint behaviors;
*/
static inline void bitset_set(borrowed(Bitset*) bs, usize i) {
    if (!bs) { return; }
    require_msg(i < bs->capacity, "bitset_set: index out of range");
    bs->words[bitset_word_of(i)] |= bitset_mask_of(i);
}

/**
 * @brief Clears bit i (makes it 0)
 *
 * NULL bs is a no-op. Out-of-bounds i on a valid bs is a contract violation.
 *
 * @pre i < bs->capacity (when bs != NULL)
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(1)
 */
/*@ requires bs == \null || (bitset_mut(bs) && bitset_pad(bs));
    requires bs == \null || bs->words != \null;
    requires bs == \null || i < bs->capacity;

    behavior null:
      assumes bs == \null;
      assigns \nothing;

    behavior live:
      assumes bs != \null;
      assigns bs->words[bitset_word_l(i)];
      ensures !bitset_bit_set(bs, i);
      ensures \forall integer k;
          0 <= k < bs->word_count && k != bitset_word_l(i)
              ==> bs->words[k] == \old(bs->words[k]);
      ensures bitset_pad(bs);
      ensures bitset_mut(bs);

    complete behaviors;
    disjoint behaviors;
*/
static inline void bitset_clear(borrowed(Bitset*) bs, usize i) {
    if (!bs) { return; }
    require_msg(i < bs->capacity, "bitset_clear: index out of range");
    bs->words[bitset_word_of(i)] &= ~bitset_mask_of(i);
}

/**
 * @brief Toggles bit i (0 → 1, 1 → 0)
 *
 * NULL bs is a no-op. Out-of-bounds i on a valid bs is a contract violation.
 *
 * @pre i < bs->capacity (when bs != NULL)
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(1)
 */
/*@ requires bs == \null || (bitset_mut(bs) && bitset_pad(bs));
    requires bs == \null || bs->words != \null;
    requires bs == \null || i < bs->capacity;

    behavior null:
      assumes bs == \null;
      assigns \nothing;

    behavior live:
      assumes bs != \null;
      assigns bs->words[bitset_word_l(i)];
      ensures bitset_bit_set(bs, i) <==> !\old(bitset_bit_set(bs, i));
      ensures \forall integer k;
          0 <= k < bs->word_count && k != bitset_word_l(i)
              ==> bs->words[k] == \old(bs->words[k]);
      ensures bitset_pad(bs);
      ensures bitset_mut(bs);

    complete behaviors;
    disjoint behaviors;
*/
static inline void bitset_toggle(borrowed(Bitset*) bs, usize i) {
    if (!bs) { return; }
    require_msg(i < bs->capacity, "bitset_toggle: index out of range");
    bs->words[bitset_word_of(i)] ^= bitset_mask_of(i);
}

/**
 * @brief Returns true if bit i is set
 *
 * NULL bs returns false. Out-of-bounds i on a valid bs is a contract violation.
 *
 * @pre i < bs->capacity (when bs != NULL)
 * @return false if bs == NULL
 * Performance: O(1)
 */
/* Single-clause, no behaviors: \result is false for NULL and the conjunction
   says so directly. Same posture as deque_int_is_full's "true for NULL,
   exactly as is_empty is. Stated, not smoothed over." */
/*@ requires bs == \null || bitset_view(bs);
    requires bs == \null || bs->words != \null;
    requires bs == \null || i < bs->capacity;
    assigns \nothing;
    ensures \result <==> (bs != \null && bitset_bit_set(bs, i));
*/
static inline bool bitset_test(borrowed(const Bitset*) bs, usize i) {
    if (!bs) { return false; }
    require_msg(i < bs->capacity, "bitset_test: index out of range");
    return (bs->words[bitset_word_of(i)] & bitset_mask_of(i)) != 0u;
}

/**
 * @brief Sets bit i to value (true = 1, false = 0)
 *
 * NULL bs is a no-op. Out-of-bounds i on a valid bs is a contract violation.
 *
 * @pre i < bs->capacity (when bs != NULL)
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(1)
 */
/* Pure delegation to set / clear, so this should discharge from their
   contracts with no new reasoning — a cheap check that the two are stated
   strongly enough to compose. */
/*@ requires bs == \null || (bitset_mut(bs) && bitset_pad(bs));
    requires bs == \null || bs->words != \null;
    requires bs == \null || i < bs->capacity;

    behavior null:
      assumes bs == \null;
      assigns \nothing;

    behavior live:
      assumes bs != \null;
      assigns bs->words[bitset_word_l(i)];
      ensures bitset_bit_set(bs, i) <==> value;
      ensures \forall integer k;
          0 <= k < bs->word_count && k != bitset_word_l(i)
              ==> bs->words[k] == \old(bs->words[k]);
      ensures bitset_pad(bs);
      ensures bitset_mut(bs);

    complete behaviors;
    disjoint behaviors;
*/
static inline void bitset_assign(borrowed(Bitset*) bs, usize i, bool value) {
    if (value) { bitset_set(bs, i); }
    else       { bitset_clear(bs, i); }
}

/* ════════════════════════════════════════════════════════════════════════════
   Bulk operations — O(n/64)
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Clears all bits
 *
 * NULL bs is a no-op.
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(n/64)
 */
static inline void bitset_clear_all(borrowed(Bitset*) bs) {
    if (!bs || !bs->words) { return; }
    mem_zero(bs->words, bs->word_count * sizeof(u64));
}

/**
 * @brief Sets all bits within capacity
 *
 * NULL bs is a no-op.
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(n/64)
 */
static inline void bitset_set_all(borrowed(Bitset*) bs) {
    if (!bs || !bs->words) { return; }
    mem_set(bs->words, 0xFF, bs->word_count * sizeof(u64));
    bitset_clear_padding(bs);
}

/**
 * @brief Inverts all bits within capacity
 *
 * NULL bs is a no-op.
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(n/64)
 */
/* ── GROUP 2 PROBE, VERIFY-020 ───────────────────────────────────────────────
   bitset_not is the simplest loop in the header: one pass, one write per
   word, then clear_padding. It is contracted here ALONE, ahead of the other
   nine loops, to answer one question with a measurement instead of a fourth
   theory:

       are bitset_mut / bitset_pad affordable when mentioned in a LOOP
       INVARIANT, given that they already time out on straight-line
       postconditions?

   Three guesses (predicate size, a missing lemma, integer division) each
   cost a multi-hour run and each was wrong. This probe costs the same one
   run and cannot be wrong about the thing it measures, because it is the
   thing itself rather than a proxy for it.

   READING IT
     invariant proves            -> the predicates survive loop contexts;
                                    write the remaining nine invariants
     invariant times out         -> they do not; Group 2 needs a different
                                    specification style (most likely: state
                                    the frame pointwise over words and keep
                                    bitset_mut out of the invariant entirely)
     Failed / Invalid            -> an invariant below is FALSE, not hard;
                                    that outranks everything else in the log

   The invariant is deliberately MINIMAL. It states the pointwise effect and
   the frame, and it does NOT carry bitset_mut, because carrying it is
   precisely what is in question. bitset_pad is not an invariant of the loop
   at all -- the loop BREAKS the padding invariant by inverting bits above
   capacity, and clear_padding restores it afterwards. That is why the
   postcondition asserts bitset_pad while the invariant does not.
   ──────────────────────────────────────────────────────────────────────────── */
/*@ requires bs == \null || bitset_mut(bs);
    requires bs == \null || bs->words != \null;

    behavior null:
      assumes bs == \null || bs->words == \null;
      assigns \nothing;

    behavior live:
      assumes bs != \null && bs->words != \null;
      assigns bs->words[0 .. bs->word_count - 1];
      ensures \forall integer k;
          0 <= k < bs->word_count ==> bs->words[k] == ~\old(bs->words[k]);
      ensures bitset_pad(bs);

    complete behaviors;
    disjoint behaviors;
*/
static inline void bitset_not(borrowed(Bitset*) bs) {
    if (!bs || !bs->words) { return; }
    /*@ loop invariant 0 <= w <= bs->word_count;
        loop invariant \forall integer k;
            0 <= k < w ==> bs->words[k] == ~\at(bs->words[k], Pre);
        loop invariant \forall integer k;
            w <= k < bs->word_count ==> bs->words[k] == \at(bs->words[k], Pre);
        loop assigns w, bs->words[0 .. bs->word_count - 1];
        loop variant bs->word_count - w;
    */
    for (usize w = 0; w < bs->word_count; w++) {
        bs->words[w] = ~bs->words[w];
    }
    bitset_clear_padding(bs);
}

/**
 * @brief AND: bs &= other (in-place)
 *
 * NULL bs or other is a no-op.
 * Operates on min(bs->word_count, other->word_count) words.
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(n/64)
 */
static inline void bitset_and(borrowed(Bitset*) bs, borrowed(const Bitset*) other) {
    if (!bs || !other) { return; }
    usize n = (bs->word_count < other->word_count) ? bs->word_count : other->word_count;
    for (usize w = 0; w < n; w++) {
        bs->words[w] &= other->words[w];
    }
    /* words beyond other's range become 0 — AND with implicit 0 */
    for (usize w = n; w < bs->word_count; w++) {
        bs->words[w] = 0;
    }
}

/**
 * @brief OR: bs |= other (in-place)
 *
 * NULL bs or other is a no-op.
 * Operates on min(bs->word_count, other->word_count) words.
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(n/64)
 */
static inline void bitset_or(borrowed(Bitset*) bs, borrowed(const Bitset*) other) {
    if (!bs || !other) { return; }
    usize n = (bs->word_count < other->word_count) ? bs->word_count : other->word_count;
    for (usize w = 0; w < n; w++) {
        bs->words[w] |= other->words[w];
    }
}

/**
 * @brief XOR: bs ^= other (in-place)
 *
 * NULL bs or other is a no-op.
 * Operates on min(bs->word_count, other->word_count) words.
 *
 * Lifetime (CANON_LIFETIME_DEBUG): does NOT restamp. Same rationale as set.
 *
 * Performance: O(n/64)
 */
static inline void bitset_xor(borrowed(Bitset*) bs, borrowed(const Bitset*) other) {
    if (!bs || !other) { return; }
    usize n = (bs->word_count < other->word_count) ? bs->word_count : other->word_count;
    for (usize w = 0; w < n; w++) {
        bs->words[w] ^= other->words[w];
    }
    bitset_clear_padding(bs);
}

/* ════════════════════════════════════════════════════════════════════════════
   Query — O(n/64)
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Returns the number of set bits (popcount)
 *
 * NULL bs returns 0.
 * Uses bits_popcount from core/primitives/bits.h per word.
 * Performance: O(n/64)
 */
static inline usize bitset_count(borrowed(const Bitset*) bs) {
    if (!bs || !bs->words) { return 0; }
    usize total = 0;
    for (usize w = 0; w < bs->word_count; w++) {
        total += (usize)bits_popcount(bs->words[w]);
    }
    return total;
}

/**
 * @brief Returns true if no bits are set
 *
 * NULL bs returns true.
 * Performance: O(n/64)
 */
static inline bool bitset_is_empty(borrowed(const Bitset*) bs) {
    if (!bs || !bs->words) { return true; }
    for (usize w = 0; w < bs->word_count; w++) {
        if (bs->words[w] != 0u) { return false; }
    }
    return true;
}

/**
 * @brief Returns true if all bits within capacity are set
 *
 * NULL bs returns false.
 * Performance: O(n/64)
 */
static inline bool bitset_is_full(borrowed(const Bitset*) bs) {
    if (!bs || !bs->words) { return false; }
    return bitset_count(bs) == bs->capacity;
}

/**
 * @brief Returns true if bs and other have no bits in common (disjoint)
 *
 * NULL bs or other returns true.
 * Performance: O(n/64)
 */
static inline bool bitset_is_disjoint(borrowed(const Bitset*) bs,
                                      borrowed(const Bitset*) other) {
    if (!bs || !other) { return true; }
    usize n = (bs->word_count < other->word_count) ? bs->word_count : other->word_count;
    for (usize w = 0; w < n; w++) {
        if ((bs->words[w] & other->words[w]) != 0u) { return false; }
    }
    return true;
}

/**
 * @brief Returns the capacity (total number of usable bits)
 *
 * NULL bs returns 0.
 * Performance: O(1)
 */
static inline usize bitset_capacity(borrowed(const Bitset*) bs) {
    return bs ? bs->capacity : 0u;
}

/* ════════════════════════════════════════════════════════════════════════════
   Search — raw sentinel variants — O(n/64)
   Used internally by BITSET_FOR_EACH. Prefer the option variants in new code.
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Returns the index of the first set bit, or BITSET_NPOS if none
 *
 * NULL bs returns BITSET_NPOS.
 * Uses bits_ctz (count trailing zeros) from core/primitives/bits.h.
 *
 * Prefer bitset_find_first_option() in new code.
 *
 * Performance: O(n/64) worst case
 */
static inline usize bitset_find_first(borrowed(const Bitset*) bs) {
    if (!bs || !bs->words) { return BITSET_NPOS; }
    for (usize w = 0; w < bs->word_count; w++) {
        if (bs->words[w] != 0u) {
            usize bit = w * BITSET_BITS_PER_WORD + (usize)bits_ctz(bs->words[w]);
            return (bit < bs->capacity) ? bit : BITSET_NPOS;
        }
    }
    return BITSET_NPOS;
}

/**
 * @brief Returns the index of the next set bit after position prev, or BITSET_NPOS
 *
 * NULL bs returns BITSET_NPOS.
 *
 * Prefer bitset_find_next_option() in new code.
 * Used for iterating over set bits via BITSET_FOR_EACH:
 * ```c
 * for (usize i = bitset_find_first(&bs);
 *      i != BITSET_NPOS;
 *      i = bitset_find_next(&bs, i)) {
 *     // process bit i
 * }
 * ```
 *
 * Performance: O(n/64) worst case per call
 */
static inline usize bitset_find_next(borrowed(const Bitset*) bs, usize prev) {
    if (!bs || !bs->words || (prev >= bs->capacity)) { return BITSET_NPOS; }

    usize start = prev + 1u;
    if (start >= bs->capacity) { return BITSET_NPOS; }

    usize w   = bitset_word_of(start);
    usize bit = start % BITSET_BITS_PER_WORD;

    /* mask off bits before 'start' in the first word */
    u64 word = bs->words[w] >> bit;
    if (word != 0u) {
        usize found = w * BITSET_BITS_PER_WORD + bit + (usize)bits_ctz(word);
        return (found < bs->capacity) ? found : BITSET_NPOS;
    }

    for (w = w + 1u; w < bs->word_count; w++) {
        if (bs->words[w] != 0u) {
            usize found = w * BITSET_BITS_PER_WORD + (usize)bits_ctz(bs->words[w]);
            return (found < bs->capacity) ? found : BITSET_NPOS;
        }
    }
    return BITSET_NPOS;
}

/**
 * @brief Returns the index of the last set bit, or BITSET_NPOS if none
 *
 * NULL bs returns BITSET_NPOS.
 * Uses bits_clz (count leading zeros) from core/primitives/bits.h.
 *
 * Prefer bitset_find_last_option() in new code.
 *
 * Performance: O(n/64) worst case
 */
static inline usize bitset_find_last(borrowed(const Bitset*) bs) {
    if (!bs || !bs->words) { return BITSET_NPOS; }
    usize w = bs->word_count;
    while (w-- > 0) {
        if (bs->words[w] != 0u) {
            usize bit = w * BITSET_BITS_PER_WORD +
                        (BITSET_BITS_PER_WORD - 1u - (usize)bits_clz(bs->words[w]));
            return (bit < bs->capacity) ? bit : BITSET_NPOS;
        }
    }
    return BITSET_NPOS;
}

/* ════════════════════════════════════════════════════════════════════════════
   Search — option_usize variants (preferred) — O(n/64)
   option_usize is instantiated by this header; see CANON_OPTION_USIZE_DEFINED.
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Returns the index of the first set bit as option_usize
 *
 * NULL bs or empty bitset returns None.
 *
 * @return Some(index) if a set bit exists, None otherwise
 *
 * Performance: O(n/64) worst case
 */
static inline option_usize bitset_find_first_option(borrowed(const Bitset*) bs) {
    usize raw = bitset_find_first(bs);
    if (raw == BITSET_NPOS) { return option_usize_none(); }
    return option_usize_some(raw);
}

/**
 * @brief Returns the index of the next set bit after prev as option_usize
 *
 * NULL bs or no further set bits returns None.
 *
 * Used for option-style iteration:
 * ```c
 * option_usize cur = bitset_find_first_option(&bs);
 * while (option_usize_is_some(cur)) {
 *     usize i = option_usize_unwrap(cur);
 *     // process bit i
 *     cur = bitset_find_next_option(&bs, i);
 * }
 * ```
 *
 * @return Some(index) if a further set bit exists, None otherwise
 *
 * Performance: O(n/64) worst case per call
 */
static inline option_usize bitset_find_next_option(borrowed(const Bitset*) bs, usize prev) {
    usize raw = bitset_find_next(bs, prev);
    if (raw == BITSET_NPOS) { return option_usize_none(); }
    return option_usize_some(raw);
}

/**
 * @brief Returns the index of the last set bit as option_usize
 *
 * NULL bs or empty bitset returns None.
 *
 * @return Some(index) if a set bit exists, None otherwise
 *
 * Performance: O(n/64) worst case
 */
static inline option_usize bitset_find_last_option(borrowed(const Bitset*) bs) {
    usize raw = bitset_find_last(bs);
    if (raw == BITSET_NPOS) { return option_usize_none(); }
    return option_usize_some(raw);
}

/* ════════════════════════════════════════════════════════════════════════════
   View — O(1)
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Returns a bytes_t view over the raw backing words
 *
 * NULL bs returns bytes_empty().
 * Covers the entire word array including any padding bits (always zero).
 * Useful for serialization, hashing, or passing to byte-level APIs.
 * Non-owning — do not free the returned bytes_t.ptr.
 *
 * This is the untracked view. For substrate-aware views that participate
 * in lifetime checking, use bitset_as_borrowed_bytes() instead.
 *
 * Performance: O(1)
 */
static inline bytes_t bitset_as_bytes(borrowed(Bitset*) bs) {
    if (!bs || !bs->words) { return bytes_empty(); }
    return bytes_from(bs->words, bs->word_count * sizeof(u64));
}

/**
 * @brief Read-only twin of bitset_as_bytes() — see API-001
 */
static inline cbytes_t bitset_as_cbytes(borrowed(const Bitset*) bs) {
    if (!bs || !bs->words) { return cbytes_empty(); }
    return cbytes_from(bs->words, bs->word_count * sizeof(u64));
}

/**
 * @brief Returns a tracked borrowed_bytes view over the raw backing words
 *
 * Like bitset_as_bytes(), but the returned view captures the Bitset's
 * lifetime token (under CANON_LIFETIME_DEBUG). After bitset_close(), any
 * borrowed_bytes_get() on the returned view fires require_msg.
 *
 * NULL bs returns borrowed_bytes_empty().
 *
 * In release builds (CANON_LIFETIME_DEBUG undefined), the returned
 * borrowed_bytes carries source_lt == NULL — the check is a no-op and
 * the view behaves exactly like wrapping bitset_as_bytes() in
 * borrowed_bytes_from(bytes.ptr, bytes.len, bs).
 *
 * @param bs Bitset to view (NULL-safe)
 * @return borrowed_bytes covering the entire word array
 *
 * Performance: O(1)
 */
static inline borrowed_bytes bitset_as_borrowed_bytes(borrowed(const Bitset*) bs) {
    if (!bs || !bs->words) { return borrowed_bytes_empty(); }
    return borrowed_bytes_from_lifetime(
        bs->words,
        bs->word_count * sizeof(u64),
#ifdef CANON_LIFETIME_DEBUG
        &bs->lt,
#else
        NULL,
#endif
        bs);
}

/* ════════════════════════════════════════════════════════════════════════════
   Iteration macro
   ════════════════════════════════════════════════════════════════════════════ */

/**
 * @def BITSET_FOR_EACH(bs_ptr, idx_var)
 * @brief Iterates over all set bit indices in a Bitset
 *
 * Uses the raw sentinel-returning find functions internally for zero overhead
 * in tight loops. For option-style manual iteration use
 * bitset_find_first_option / bitset_find_next_option instead.
 *
 * @param bs_ptr  Pointer to Bitset
 * @param idx_var usize variable name to use as the loop index
 *
 * Example:
 * ```c
 * BITSET_FOR_EACH(&bs, i) {
 *     printf("bit %zu is set\n", i);
 * }
 * ```
 */
/* cppcheck-suppress misra-c2012-20.7 ; MISRA-DEV-012 */
#define BITSET_FOR_EACH(bs_ptr, idx_var) \
    for (usize idx_var = bitset_find_first(bs_ptr); \
         idx_var != BITSET_NPOS; \
         idx_var = bitset_find_next((bs_ptr), idx_var))

#endif /* CANON_DATA_BITSET_H */
