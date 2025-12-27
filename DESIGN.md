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

- Programs are written in terms of **transformations**, not hidden side effects:

    input → transform → output


  This is not strict functional programming, but a guideline to improve
  readability and predictability.

---

## Literate Readability

- Minimal semantic distance between the programmer’s intent and the code.
- Fewer rules to memorize.
- No invisible side effects.
- Code should read like a clear explanation of its purpose.

---

## Foundational vs Optional Modules

**Foundational modules**:

- Appear in most non-trivial programs.
- Do not encode ideology (OO, async, inheritance, etc.).
- Reduce mechanical noise.
- Make behavior explicit.
- Compose cleanly with other modules.

**Examples**: arena allocation, option/result types, bounded collections, explicit iteration.

**Optional modules**:

- Helpful but isolated.
- May depend on foundational modules.
- Must not break the dependency rule.



