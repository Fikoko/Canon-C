# Canon-C

A collection of small, explicit C libraries designed to improve readability,
semantic clarity, and development speed (via reduced cognitive load),
while preserving C’s performance and predictability.

This project treats **C as an execution backend**, not as a semantic authority.

---

## Motivation

C is fast, portable, and honest, but its native semantics are low-level and
mechanical. Writing non-trivial programs in C often requires memorizing
patterns, rules, and boilerplate such as manual memory management,
ownership conventions, error flags, and loop idioms.

These details obscure intent and slow development.

Many modern languages solve this by embedding abstractions directly into
the language. While powerful, this increases semantic complexity,
hides behavior, and raises the cost of mastery.

This project takes a different path.

C is left untouched.  
Meaning is added through **libraries**, not syntax.

The goal is to enable **literate, intention-revealing C code** while
preserving performance, portability, and transparency.

Readability is treated as a real engineering constraint.

---

## Core Idea

Everything is a library.  
Everything is explicit.  

C is used as a **low-level target**, while higher-level semantics
(memory, lifetime, failure, data flow, transformation)
are expressed through small, composable modules.

You do not inherit semantics from the language.  
You **select** semantics by choosing which headers to include.

---

## Design Philosophy

- Everything is optional
- Everything is explicit
- No hidden allocation
- No implicit ownership
- No global state
- No framework coupling
- No required runtime
- No language extensions
- No clever tricks

If behavior cannot be understood by reading the header, it does not belong.

Abstractions must clarify behavior, not conceal it.

---

## Included Modules (Current)

- **core/**
  - `arena.h` — explicit linear allocation
  - `memory.h` — allocation helpers and contracts
- **data/**
  - `vec.h` — dynamic vector (automatic & manual modes)
- **semantics/**
  - `option.h` — explicit optional values
  - `result.h` — explicit success/error values
- **algo/**
  - `map.h` — functional-style data transformation
- **util/**
  - `string.h` — string utilities (automatic & manual)
  - `log.h` — minimal logging

All modules are **header-only** and require no runtime or build system integration.

---

## What This Is

- A curated set of **foundational C modules**
- A shared structure and discipline for semantic libraries
- A functional-style semantic vocabulary built *on top of* C
- Usable from both **C and C++**
- Optimized for readability, predictability, and control

---

## What This Is Not

- Not a replacement for C++
- Not a new programming language
- Not object-oriented
- Not macro-heavy metaprogramming
- Not a framework
- Not opinionated about application architecture

This project does not compete with languages.  
It avoids them.
