/**
 * @file const_correctness_test.c
 * @brief Regression guard: read-only accessors must accept const objects.
 *
 * Before the fix accompanying this test, ten accessors across arena, pool,
 * bitset, priority_queue and stringbuf took a `const X*` and returned a
 * MUTABLE `bytes_t` (or, for pool_get, a raw `void*`). A caller holding only
 * a const pointer could obtain a writable view and mutate the object —
 * demonstrated at the time by writing 0xEE through a `const Arena*`. The
 * const qualifier was decorative.
 *
 * The convention is now the one slice.h and array.h already used
 * (docs/design-decisions.md, API-001):
 *
 *     X_as_bytes (X*)        -> bytes_t     mutable in, mutable out
 *     X_as_cbytes(const X*)  -> cbytes_t    const in,   read-only out
 *
 * This file pins the const half: every `_cbytes` accessor is called through
 * a genuinely `const`-qualified pointer. If someone reintroduces a mutable
 * return on a const parameter, the mutable half stops compiling against
 * these const objects and this test fails to build.
 *
 * The mutable half is covered by the existing per-module tests.
 */
#define CANON_CONTRACT_IMPL

#include "core/arena.h"
#include "core/pool.h"
#include "core/slice.h"
#include "semantics/option/option.h"

/* CANON_OPTION(usize) must be instantiated before including bitset.h —
   bitset.h uses option_usize but does not instantiate it itself. */
CANON_OPTION(usize)

#include "data/bitset.h"
#include "data/priority_queue.h"
#include "data/stringbuf.h"

#include <stdio.h>

#define CHECK(cond, msg)                                                      \
    do {                                                                      \
        if (!(cond)) { printf("FAIL: %s\n", (msg)); return 1; }                \
    } while (0)

static int cmp_int(const void* a, const void* b, void* ctx) {
    const int x = *(const int*)a;
    const int y = *(const int*)b;
    (void)ctx;
    return (x < y) ? -1 : ((x > y) ? 1 : 0);
}

int main(void) {
    static u8 arena_buf[256];
    static u8 pool_buf[256];
    static u64 words[4];

    Arena arena;
    arena_init(&arena, arena_buf, sizeof arena_buf);
    (void)arena_alloc(&arena, 32u);

    /* Bind through const-qualified pointers: the compiler enforces the
       contract we are asserting. */
    const Arena* ca = &arena;

    cbytes_t a1 = arena_as_cbytes(ca);
    cbytes_t a2 = arena_buffer_cbytes(ca);
    cbytes_t a3 = arena_free_cbytes(ca);

    CHECK(a1.len == 32u, "arena_as_cbytes length");
    CHECK(a2.len == sizeof arena_buf, "arena_buffer_cbytes length");
    CHECK(a1.len + a3.len == a2.len, "used + free == capacity");

    Arena pool_arena;
    arena_init(&pool_arena, pool_buf, sizeof pool_buf);
    Pool pool;
    if (pool_init(&pool, &pool_arena, 8u, 4u)) {
        (void)pool_alloc(&pool);
        const Pool* cp = &pool;

        cbytes_t p1 = pool_as_cbytes(cp);
        cbytes_t p2 = pool_reserved_cbytes(cp);

        /* object_size is aligned up by pool_init, so derive rather than
           hardcode: used objects occupy used * object_size bytes. */
        CHECK(p1.len == pool_used(cp) * pool_object_size(cp),
              "pool_as_cbytes covers used objects");
        CHECK(p2.len == pool_capacity(cp) * pool_object_size(cp),
              "pool_reserved_cbytes covers the whole reserved region");
        CHECK(p2.len >= p1.len, "reserved covers at least used");

        /* pool_get_const remains the read-only element accessor. */
        CHECK(pool_get_const(cp, 0u) != NULL, "pool_get_const on const Pool");
    }

    Bitset bs;
    bitset_init(&bs, words, 4u * 64u);
    const Bitset* cb = &bs;
    cbytes_t b1 = bitset_as_cbytes(cb);
    CHECK(b1.len == 4u * sizeof(u64), "bitset_as_cbytes length");

    StringBuf sb;
    static char sb_buf[64];
    stringbuf_init_buffer(&sb, sb_buf, sizeof sb_buf);
    (void)stringbuf_append_str(&sb, str_from_cstr("abc"));
    const StringBuf* csb = &sb;
    cbytes_t s1 = stringbuf_as_cbytes(csb);
    cbytes_t s2 = stringbuf_buffer_cbytes(csb);
    CHECK(s1.len == 3u, "stringbuf_as_cbytes length");
    CHECK(s2.len == sizeof sb_buf, "stringbuf_buffer_cbytes length");

    /* ── Guard paths ──────────────────────────────────────────────────────
       The _cbytes accessors added alongside this test carry NULL and
       empty-state guards. Exercising only the happy path would leave those
       conditions uncovered and drag the MC/DC report down for every file
       they live in, so both outcomes of each guard are taken here.

       arena's three _cbytes accessors guard with require_msg(), which the
       coverage build compiles to ((void)0) via CANON_NO_REQUIRE — they are
       therefore not NULL-callable and emit no condition. arena_free_cbytes
       does have a real branch (offset >= capacity), taken below. */

    CHECK(pool_as_cbytes(NULL).len == 0u,        "pool_as_cbytes(NULL)");
    CHECK(pool_reserved_cbytes(NULL).len == 0u,  "pool_reserved_cbytes(NULL)");
    CHECK(bitset_as_cbytes(NULL).len == 0u,      "bitset_as_cbytes(NULL)");
    CHECK(pq_as_cbytes(NULL).len == 0u,          "pq_as_cbytes(NULL)");
    CHECK(stringbuf_as_cbytes(NULL).len == 0u,   "stringbuf_as_cbytes(NULL)");
    CHECK(stringbuf_buffer_cbytes(NULL).len == 0u, "stringbuf_buffer_cbytes(NULL)");

    /* Empty-pool path: used == 0 is a distinct guard from pool == NULL. */
    {
        static u8 e_buf[128];
        Arena e_arena;
        arena_init(&e_arena, e_buf, sizeof e_buf);
        Pool e_pool;
        if (pool_init(&e_pool, &e_arena, 8u, 2u)) {
            const Pool* cep = &e_pool;
            CHECK(pool_as_cbytes(cep).len == 0u, "pool_as_cbytes on empty pool");
            CHECK(pool_reserved_cbytes(cep).len > 0u,
                  "pool_reserved_cbytes non-empty on empty pool");
        }
    }

    /* Exhausted arena: drives arena_free_cbytes' offset >= capacity branch. */
    {
        static u8 f_buf[64];
        Arena f_arena;
        arena_init(&f_arena, f_buf, sizeof f_buf);
        while (arena_alloc(&f_arena, 8u) != NULL) { /* fill it */ }
        const Arena* cfa = &f_arena;
        cbytes_t rest = arena_free_cbytes(cfa);
        CHECK(rest.len < sizeof f_buf, "arena_free_cbytes on exhausted arena");
    }

    /* A PriorityQueue that exists but is empty: len == 0 guard. */
    {
        static int pq_buf[16];
        PriorityQueue pq;
        pq_init(&pq, pq_buf, 16u, sizeof(int), cmp_int, NULL);
        const PriorityQueue* cpq = &pq;
        CHECK(pq_as_cbytes(cpq).len == 0u, "pq_as_cbytes on empty queue");
    }

    printf("OK  const_correctness_test  (all assertions passed)\n");
    return 0;
}
