# Canon-C-Library
This repository is a collection of small, explicit C utilities that form a coherent semantic layer on top of C.

# non semantic "c"

A minimal, functional, literate semantic layer for C.

This repository collects small, explicit C utilities that allow C to be used as an **execution backend**, while higher-level meaning is expressed through a simple, functional-style library.

The goal is not to extend C into another language, but to **reduce the semantic and cognitive burden of programming** by replacing low-level mechanics (manual allocation, pointer bookkeeping, loop scaffolding) with clear, intention-revealing operations.

C provides execution.  
Semantics live in the library.

---

## Motivation

C is fast, predictable, and transparent — but its default semantics force the programmer to constantly manage details that are mechanical rather than meaningful.

This project explores a different approach:

- keep C’s performance and simplicity
- avoid language-level abstraction systems
- build a small, explicit semantic layer as a **library**
- favor functional data flow (input → output)
- support literate, readable code
- make abstractions opt-in, inspectable, and replaceable

---

## Core Principles

- **Library, not language**
- **Zero-cost abstractions**
- **Functional-style APIs**
- **Explicit memory models** (arena-based allocation)
- **No mandatory `malloc` / `free` in user code**
- **Readable, intention-first code**
- **Minimal surface area**

---

## What This Is Not

- Not a framework
- Not object-oriented C
- Not a C++ or Rust replacement
- Not a new language
- Not an STL clone

---

## Scope

This repository will incrementally collect:

- arena allocators
- semantic containers (`Vec`, `Option`, `Result`)
- functional utilities (`map`, `filter`, `fold`, `range`)
- small, composable helpers

Each component is independent and optional.

---

## Philosophy

> Abstractions should be chosen, not inherited.

---

## Status

Early exploration.  
Expect small commits, minimal dependencies, and evolving ideas.
