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

/* bitset.h is deliberately NOT included here. It requires CANON_OPTION(usize)
   to be instantiated in the including translation unit, and that macro
   expands ~14 static inline functions INTO THIS FILE. Clang exempts
   header-defined functions from -Wunused-function but not main-file ones, so
   instantiating a type this test does not otherwise use breaks the clang
   builds. bitset_as_cbytes is covered by test_bitset_cbytes_accessor in
   test/data/bitset_test.c, which binds through a const Bitset* and therefore
   carries the same const guarantee. */
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

    Arena arena;
    arena_init(&arena, arena_buf, sizeof arena_buf);
    (void)arena_alloc(&arena, 32u);

    /* Bind through const-qualified pointers: the compiler enforces the
       contract this file exists to assert. Every length is DERIVED from the
       mutable twin or from an accessor — never hardcoded. arena_alloc aligns,
       so the offset after a 32-byte allocation is 32 PLUS whatever padding
       the runtime address of arena_buf requires, which is platform-dependent
       and not something this test should predict. */
    const Arena* ca = &arena;

    CHECK(arena_as_cbytes(ca).len     == arena_as_bytes(&arena).len,
          "arena_as_cbytes agrees with arena_as_bytes");
    CHECK(arena_buffer_cbytes(ca).len == arena_buffer_bytes(&arena).len,
          "arena_buffer_cbytes agrees with its twin");
    CHECK(arena_free_cbytes(ca).len   == arena_free_bytes(&arena).len,
          "arena_free_cbytes agrees with its twin");
    CHECK(arena_buffer_cbytes(ca).len == arena_capacity(ca),
          "buffer view spans the whole capacity");
    CHECK(arena_as_cbytes(ca).len + arena_free_cbytes(ca).len
              == arena_capacity(ca),
          "used + free == capacity");

    Arena pool_arena;
    arena_init(&pool_arena, pool_buf, sizeof pool_buf);
    Pool pool;
    if (pool_init(&pool, &pool_arena, 8u, 4u)) {
        (void)pool_alloc(&pool);
        const Pool* cp = &pool;

        CHECK(pool_as_cbytes(cp).len       == pool_as_bytes(&pool).len,
              "pool_as_cbytes agrees with its twin");
        CHECK(pool_reserved_cbytes(cp).len == pool_reserved_bytes(&pool).len,
              "pool_reserved_cbytes agrees with its twin");
        CHECK(pool_as_cbytes(cp).len == pool_used(cp) * pool_object_size(cp),
              "pool_as_cbytes covers used objects");
        CHECK(pool_reserved_cbytes(cp).len >= pool_as_cbytes(cp).len,
              "reserved covers at least used");

        /* pool_get_const remains the read-only element accessor. */
        CHECK(pool_get_const(cp, 0u) != NULL, "pool_get_const on const Pool");
    }

    StringBuf sb;
    static char sb_buf[64];
    stringbuf_init_buffer(&sb, sb_buf, sizeof sb_buf);
    (void)stringbuf_append_str(&sb, str_from_cstr("abc"));
    const StringBuf* csb = &sb;

    CHECK(stringbuf_as_cbytes(csb).len     == stringbuf_as_bytes(&sb).len,
          "stringbuf_as_cbytes agrees with its twin");
    CHECK(stringbuf_buffer_cbytes(csb).len == stringbuf_buffer_bytes(&sb).len,
          "stringbuf_buffer_cbytes agrees with its twin");
    CHECK(stringbuf_as_cbytes(csb).len == 3u,
          "stringbuf_as_cbytes reflects appended length");

    /* ── Guard paths ──────────────────────────────────────────────────────
       The _cbytes accessors carry NULL and empty-state guards. Exercising
       only the happy path would leave those conditions uncovered. arena's
       accessors guard with require_msg(), which the coverage build compiles
       out via CANON_NO_REQUIRE — they are not NULL-callable and emit no
       condition, so only the container accessors are probed with NULL. */
    CHECK(pool_as_cbytes(NULL).len == 0u,          "pool_as_cbytes(NULL)");
    CHECK(pool_reserved_cbytes(NULL).len == 0u,    "pool_reserved_cbytes(NULL)");
    CHECK(pq_as_cbytes(NULL).len == 0u,            "pq_as_cbytes(NULL)");
    CHECK(stringbuf_as_cbytes(NULL).len == 0u,     "stringbuf_as_cbytes(NULL)");
    CHECK(stringbuf_buffer_cbytes(NULL).len == 0u, "stringbuf_buffer_cbytes(NULL)");

    /* Empty pool: used == 0 is a distinct guard from pool == NULL. */
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
        while (arena_alloc(&f_arena, 8u) != NULL) { /* fill */ }
        const Arena* cfa = &f_arena;
        CHECK(arena_free_cbytes(cfa).len == arena_free_bytes(&f_arena).len,
              "arena_free_cbytes on exhausted arena");
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
