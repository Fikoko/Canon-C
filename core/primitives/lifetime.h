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

#ifndef CANON_CORE_PRIMITIVES_LIFETIME_H
#define CANON_CORE_PRIMITIVES_LIFETIME_H

#include <stdbool.h>
#include "core/primitives/types.h"

/**
 * @file core/primitives/lifetime.h
 * @brief Lifetime token types shared across owning modules
 *
 * This header is the canonical home of three small types used by every
 * Canon-C module that participates in lifetime tracking:
 *
 *   region_id_t       — 64-bit identifier for a lifetime
 *   REGION_ID_STATIC  — reserved ID for static / never-expiring borrows
 *   lifetime_t        — (id, open) pair embedded by owning modules
 *
 * Why this lives in primitives/:
 * ────────────────────────────────────────────────────────────────────────────
 * Multiple owning modules — core/arena.h, core/region.h, and (in later
 * phases) data/convenience/dynvec.h, data/convenience/dynstring.h,
 * data/convenience/smallvec.h, data/vec/, data/deque/, data/hashmap/,
 * data/stringbuf.h, data/priority_queue.h — all need to embed the same
 * (id, open) pair so that borrows can validate against any source via a
 * single const lifetime_t* pointer.
 *
 * The types must therefore have a single canonical home that all of
 * those modules can include without forming a cycle. core/primitives/
 * is the right home: it sits at the bottom of the dependency rule and
 * has no upward dependencies, so any module can include it freely.
 *
 * Why not in core/region.h:
 * ────────────────────────────────────────────────────────────────────────────
 * region.h includes core/arena.h (to reference Arena*). If arena.h had
 * to include region.h for the types, the two headers would form an
 * include cycle — region.h pulls in arena.h, arena.h pulls in region.h,
 * and whichever is parsed first sees the other half-defined. Putting
 * the shared types in primitives/ breaks the cycle: both arena.h and
 * region.h include this header without involving each other.
 *
 * Portability:
 * ────────────────────────────────────────────────────────────────────────────
 * region_id_t is u64, which is guaranteed by the Tier 0 platform
 * contract enforced in core/primitives/types.h (C99 exact-width types,
 * 8-bit bytes) — this header is therefore safe on any target where
 * types.h compiles, including 32-bit and 16-bit-size_t platforms, where
 * u64 is synthesized from multiple machine words. The 64-bit width is a
 * deliberate anti-wraparound choice, not an oversight: the per-TU
 * restamp counters that feed region_id_t (see OWN-002) must not wrap on
 * any realistic timescale, because a wrapped counter could hand a stale
 * borrow its original ID back — exactly the re-validation bug class the
 * substrate exists to catch. A 32-bit ID would wrap after 2^32 restamps
 * (~50 days at 1000 resets/sec on a long-running device); a 64-bit ID
 * does not wrap on any realistic timescale. Note that under
 * CANON_LIFETIME_DEBUG, lifetime_t occupies 16 bytes on typical ABIs
 * (8-byte id + 1-byte bool + padding to u64 alignment); default builds
 * omit the field entirely, so production builds pay nothing.
 *
 * What this header is NOT:
 * ────────────────────────────────────────────────────────────────────────────
 * - Not a lifetime-tracking runtime. The runtime check lives in
 *   core/region.h as lifetime_assert_valid() because that's where
 *   ensure_msg is available.
 * - Not gated by CANON_LIFETIME_DEBUG. The types are always defined,
 *   because Region uses region_id_t unconditionally (lifetime IDs are
 *   a release-build feature of Region, not a debug-only one). Modules
 *   that only need the types under CANON_LIFETIME_DEBUG can still
 *   include this header unconditionally — the types are tiny and cost
 *   nothing if unused.
 * - Not extensible. lifetime_t is exactly two fields. Anything richer
 *   (generation counters, per-thread tracking, dependency graphs)
 *   would belong in a different module, not by extending this one.
 *
 * Dependency rule:
 * ────────────────────────────────────────────────────────────────────────────
 * This header is in core/primitives/ and includes only <stdbool.h> and
 * core/primitives/types.h. Nothing else may be included from here.
 *
 * @sa core/region.h         — Region type and lifetime_assert_valid()
 * @sa core/arena.h          — Arena embeds lifetime_t under CANON_LIFETIME_DEBUG
 * @sa semantics/borrow.h    — borrow types that capture and validate IDs
 * @sa core/primitives/types.h — Tier 0 platform contract that guarantees u64
 */

/**
 * @brief Unique identifier for a lifetime
 *
 * For Region, derived from the Region's stack address in region_begin().
 * For Arena (and other modules under CANON_LIFETIME_DEBUG), derived from
 * the owner's address and re-stamped on every reset.
 *
 * Intended to be distinct across all simultaneously live owners.
 * Not monotonic — addresses, not sequence numbers.
 *
 * The strength of that distinctness is worth stating precisely, because the
 * derivation is `counter ^ owner-address` and the counter term is weaker
 * than it looks:
 *   - The counter has internal linkage in a header, so there is one instance
 *     PER TRANSLATION UNIT, not one per program. Two owners constructed in
 *     different TUs can draw the same counter value.
 *   - Distinctness therefore rests principally on the address term, which is
 *     sound for simultaneously live owners (distinct live objects have
 *     distinct addresses). The counter is a diversifier, not the identity.
 *   - Tokens are not unique across time: an address may be reused once an
 *     owner dies, and the counter is what makes reuse unlikely rather than
 *     impossible.
 * See docs/thread-safety.md for the concurrency contract.
 *
 * 0 is reserved as REGION_ID_STATIC ("no owner" / static lifetime).
 *
 * A valid owner never has ADDRESS 0 on any conforming C99 implementation --
 * but that is a fact about the address, and the id is not the address. The
 * id is `counter ^ address`, which is 0 whenever the counter happens to
 * equal the address, and the counter walks 1, 2, 3, ... So the id CAN be 0
 * where the address cannot, and the address argument does not establish
 * what it was previously written here to establish.
 *
 * How reachable that is depends entirely on the address map. On a hosted
 * 64-bit target the counter would have to walk to a stack or heap address
 * (order 1e14 calls) and it is infeasible. On a small target with objects
 * low in RAM it is a few thousand calls and is ordinary. Canon-C targets
 * the second kind of machine.
 *
 * The explicit guard at the end of canon_lifetime_next_id_ is therefore
 * load-bearing, not defensive belt-and-braces, and it is what discharges
 * the `\result != REGION_ID_STATIC` postcondition. Corrected 2026-09; the
 * superseded wording claimed the property followed from addresses alone.
 */
typedef u64 region_id_t;

/**
 * @brief Reserved ID — "no owner" / static lifetime.
 *
 * Borrows with this ID never expire and never fire lifetime assertions.
 * Used for borrows over static const data, string literals, and any
 * value whose lifetime exceeds every possible owner.
 */
#define REGION_ID_STATIC ((region_id_t)0)

/* ============================================================================
   Internal: lifetime-token generation (CANON_LIFETIME_DEBUG only)

   One implementation, shared by every ownership-bearing module. Before
   2026-08 each of the eleven modules carried a byte-identical private copy;
   they are now thin call sites onto this one, so the derivation cannot drift
   between modules and has a single place to be fixed.

   ── Why this is atomic ────────────────────────────────────────────────────
   Generating a token is a read-modify-write of a counter with static storage
   duration. Done non-atomically, two threads constructing owners
   concurrently can read the same counter value; since the token is
   `counter ^ address`, two owners can then receive the SAME token — and the
   token is exactly the value the borrow checks compare. A stale borrow can
   validate against a different live owner, so the check passes when it
   should fail. The mode fails silently and in the unsafe direction.

   Measured (32 threads, 1.6M tokens, fixed owner address so the address term
   cannot mask a counter collision, single-core runner):

     plain increment, -O0    0 duplicates       <- does NOT reproduce
     plain increment, -O2    150,000 (9.4%)     <- optimiser hoists the counter
     atomic,          -O2    0 duplicates

   ThreadSanitizer reports the race directly on the plain path. The -O0 row is
   the reason this is a layered fix rather than a documented caveat: a
   debug-only safety mechanism that is intact in a debug build and 9% corrupt
   in an optimised one is the worst available failure profile.

   ── Why not C11 <stdatomic.h> alone ───────────────────────────────────────
   Canon-C is C99 (CMAKE_C_STANDARD 99, REQUIRED). A C11-only fix would be
   dead code in every CI job and for every caller following the project's own
   language stance — the fix would exist behind a door the project keeps
   locked. The atomic intrinsics of the three supported compilers are
   available in C99 mode, so the ladder below reaches real builds.

   ── The ladder ────────────────────────────────────────────────────────────
     1. C11 <stdatomic.h>            standard, preferred when available
     2. GCC/Clang __atomic builtins  C99-compatible; MISRA-DEV-018
     3. MSVC _InterlockedIncrement64 C99-compatible; MISRA-DEV-018
     4. plain increment              fallback; NOT race-free, see contract

   Levels 2 and 3 are compiler extensions and are therefore disabled by
   CANON_NO_GNU_EXTENSIONS, which is how the CompCert job and any strict-C99
   build reach level 4. CANON_LIFETIME_NO_ATOMICS forces level 4 explicitly,
   which is what the CI job uses to demonstrate that the fallback really does
   race.

   CANON_LIFETIME_ATOMIC_IDS expands to 1 at levels 1-3 and 0 at level 4, so
   a caller can test which contract applies.

   relaxed ordering is deliberate and sufficient at every atomic level: the
   only property required is that concurrent increments return distinct
   values, which read-modify-write operations on one object guarantee under
   any ordering. No happens-before edge is claimed here — publishing an owner
   to another thread remains the caller's synchronisation, exactly as it is
   for every other Canon-C object.
   ============================================================================ */

#ifdef CANON_LIFETIME_DEBUG

#if defined(CANON_LIFETIME_NO_ATOMICS)
    /* Level 4 forced. */
#   define CANON_LIFETIME_ATOMIC_IDS 0
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && \
      !defined(__STDC_NO_ATOMICS__)
    /* Level 1: standard C11 atomics. */
#   include <stdatomic.h>
#   define CANON_LIFETIME_ATOMIC_IDS 1
#   define CANON_LIFETIME_ATOMIC_LEVEL_ 1
#elif !defined(CANON_NO_GNU_EXTENSIONS) && defined(__GNUC__)
    /* Level 2: GCC/Clang __atomic builtins, usable from C99. */
#   define CANON_LIFETIME_ATOMIC_IDS 1
#   define CANON_LIFETIME_ATOMIC_LEVEL_ 2
#elif !defined(CANON_NO_GNU_EXTENSIONS) && defined(_MSC_VER)
    /* Level 3: MSVC interlocked intrinsics, usable from C99. */
#   include <intrin.h>
#   define CANON_LIFETIME_ATOMIC_IDS 1
#   define CANON_LIFETIME_ATOMIC_LEVEL_ 3
#else
    /* Level 4: no atomics available. Construction must be serialised. */
#   define CANON_LIFETIME_ATOMIC_IDS 0
#endif

/* CANON_LIFETIME_ATOMIC_LEVEL_ must be defined on EVERY path, including the
 * two that select level 4, because the selection below tests it in #if.
 * Leaving it undefined there relies on C's rule that an unrecognised
 * identifier in a controlling expression evaluates to 0 — which is legal and
 * is exactly the fragility MISRA rule 20.9 prohibits. CI #<this run> found it
 * as three 20.9 findings; the fix is to define it rather than suppress them. */
#ifndef CANON_LIFETIME_ATOMIC_LEVEL_
#   define CANON_LIFETIME_ATOMIC_LEVEL_ 4
#endif

/*
 * Contract, and the configuration that checks it.
 *
 * Until 2026-09 this block carried no ACSL, deliberately: no verified
 * configuration translated it, so a contract here would have been unchecked
 * decoration — and two of the eleven superseded private copies proved the
 * point by carrying `assigns \nothing`, which is FALSE (the function writes
 * the counter) and survived precisely because nothing checked it.
 *
 * That reasoning is unchanged. What changed is the premise: the token
 * generator is the project's last unverified INSTRUMENT, so rather than
 * annotate an untranslated block, a configuration that translates it was
 * added — `frama-c-lifetime`, running with CANON_LIFETIME_DEBUG on and
 * CANON_LIFETIME_NO_ATOMICS forcing level 4.
 *
 * Levels 1-3 are excluded and stay excluded. WP has no concurrency model
 * for atomic_fetch_add_explicit, __atomic_fetch_add or
 * _InterlockedIncrement64, so the contract is gated on level 4 and asserts
 * nothing about the other three. The atomic ladder's correctness argument
 * remains the prose one in docs/thread-safety.md; this contract does not
 * extend to it and must not be read as if it did.
 *
 * What is claimed, and what is deliberately not:
 *
 *   assigns counter_        — true, and the correction of the historical
 *                             `assigns \nothing`.
 *   \result != REGION_ID_STATIC
 *                           — the property with teeth. Handing out 0 would
 *                             mark the owner as static-lifetime, so every
 *                             borrow over it would stop expiring: the
 *                             instrument would fail OPEN. The guard below is
 *                             what makes this hold, and it holds independently
 *                             of how the memory model interprets the
 *                             pointer-to-integer cast.
 *
 *   Distinctness of ids is NOT claimed and is not claimable here: it is a
 *   property over a SEQUENCE of calls, and a WP contract speaks about one.
 *   Injectivity is not merely unproved but FALSE by construction — the
 *   REGION_ID_STATIC guard maps the xor-cancelling case onto 1, which is
 *   also reachable directly, so two distinct owners can receive id 1. The
 *   collision rate is negligible; the fact is recorded because the
 *   instrument's users should not assume a property it does not have.
 */
#if defined(__FRAMAC__) && (CANON_LIFETIME_ATOMIC_LEVEL_ == 4)
/*@
  assigns counter_;
  ensures \result != REGION_ID_STATIC;
*/
#endif
static inline region_id_t canon_lifetime_next_id_(const void* owner_) {
#if CANON_LIFETIME_ATOMIC_IDS && (CANON_LIFETIME_ATOMIC_LEVEL_ == 1)
    static _Atomic region_id_t counter_ = 1;
    const region_id_t c_ = atomic_fetch_add_explicit(
        &counter_, (region_id_t)1, memory_order_relaxed);
#elif CANON_LIFETIME_ATOMIC_IDS && (CANON_LIFETIME_ATOMIC_LEVEL_ == 2)
    static region_id_t counter_ = 1;
    /* cppcheck-suppress misra-c2012-1.2 ; MISRA-DEV-018 */
    const region_id_t c_ = __atomic_fetch_add(&counter_, (region_id_t)1,
                                              __ATOMIC_RELAXED);
#elif CANON_LIFETIME_ATOMIC_IDS && (CANON_LIFETIME_ATOMIC_LEVEL_ == 3)
    static volatile long long counter_ = 1;
    /* cppcheck-suppress misra-c2012-1.2 ; MISRA-DEV-018 */
    const region_id_t c_ = (region_id_t)(_InterlockedIncrement64(&counter_) - 1);
#else
    /* Level 4. Not race-free: see docs/thread-safety.md. Callers must
       serialise construction under CANON_LIFETIME_DEBUG on this path. */
    static region_id_t counter_ = 1;
    const region_id_t c_ = counter_;
    counter_++;
#endif
    region_id_t id_ = c_ ^ (region_id_t)(uintptr_t)owner_;
    /* REGION_ID_STATIC (0) is reserved; never hand it out. Reachable when
       c_ == (uintptr_t)owner_ -- a few thousand calls on a low-address
       target, infeasible on hosted 64-bit. Discharges the postcondition;
       breaks injectivity, since 1 is also produced directly. */
    if (id_ == REGION_ID_STATIC) { id_ = (region_id_t)1; }
    return id_;
}

#endif /* CANON_LIFETIME_DEBUG */

/**
 * @brief The (id, open) pair embedded by ownership-bearing modules
 *
 * Modules embedding this:
 *   - core/arena.h         (under CANON_LIFETIME_DEBUG)
 *   - core/pool.h          (planned, Phase 3)
 *   - data/convenience     (planned, Phase 2)
 *   - data/vec, deque, hashmap, stringbuf, priority_queue (planned, Phase 3)
 *
 * Region does NOT embed lifetime_t — it has separate id/open fields by
 * historical accident. lifetime_assert_valid() takes (id, open) values
 * directly, so Region's separate fields and other modules' embedded
 * lifetime_t are interoperable at the assertion site.
 *
 * Borrows that need to validate against a source store a
 * const lifetime_t* pointing at the source's lt field plus a captured
 * id, then call lifetime_assert_valid() on read.
 *
 * The struct is a layout convention, not a clever abstraction. It exists
 * so borrows can hold a single pointer regardless of which owning struct
 * they came from.
 */
typedef struct {
    region_id_t id;
    bool        open;
} lifetime_t;

#endif /* CANON_CORE_PRIMITIVES_LIFETIME_H */