# Design Principles

This document explains the **semantic rules and behavior expectations**
for modules in the `Canon-C` ecosystem.

---

## Memory Model

- All memory comes from the caller.
- Allocators must be passed explicitly.
- Lifetimes are visible and predictable.
- Hidden `malloc`/`free` or implicit allocation is forbidden.
- Arena-style allocation is preferred when possible.

---

## Failure Model

- Failure and absence are represented explicitly as values.
- No sentinel values, global flags, or hidden control flow.
- Types such as `Option<T>` or `Result<T, E>` must be used to express potential failure.
- Every function that can fail must communicate this in its signature.

---

## Functional Data Flow

- Programs are written in terms of **transformations**, not hidden side effects.


