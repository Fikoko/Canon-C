# Design Principles

This document defines the **semantic rules and behavioral expectations**
for modules in the Canon-C ecosystem.

These rules are intentionally strict.  
If a module violates them, it does not belong.

---

## Memory Model

- All memory ownership is explicit.
- Allocators or buffers must be passed by the caller **or**
  allocation must be clearly documented in the API.
- Lifetimes must be visible and predictable.
- Hidden ownership transfer is forbidden.
- Arena-style allocation is preferred when possible.

If a function allocates internally, this must be obvious from its name,
documentation, or usage pattern.

---

## Failure Model

- Failure and absence are represented explicitly as values.
- No hidden global flags or implicit control flow.
- Types such as `Option<T>` or `Result<T, E>` are the preferred way to
  express absence or failure.
- Every function that can fail must communicate this in its signature
  or return convention.

Conventional C patterns (e.g. returning `NULL`) are acceptable when
clearly documented and unavoidable.

---

## Functional Data Flow

Programs are written in terms of **transformations**, not hidden side effects:

input → transform → output


This is not strict functional programming.
It is a readability rule that favors explicit data flow over mutation
hidden across multiple layers.

---

## Literate Readability

- Minimal semantic distance between intent and code.
- Fewer rules to memorize.
- No invisible side effects.
- Behavior must be discoverable by reading the header.

Code should read like a clear explanation of its purpose.

---

## Foundational vs Optional Modules

**Foundational modules**:

- Appear in most non-trivial programs.
- Do not encode ideology (OO, async, inheritance, etc.).
- Reduce mechanical noise.
- Make behavior explicit.
- Compose cleanly with other modules.

**Examples**:
arena allocation, option/result types, bounded collections,
explicit iteration.

**Optional modules**:

- Helpful but isolated.
- May depend on foundational modules.
- Must not impose new semantic rules.
