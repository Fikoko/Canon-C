# Canon-C

A collection of small, explicit C libraries designed to improve readability,
semantic clarity, and development speed while preserving C’s performance and
predictability.

This project treats **C as an execution backend**, not as a semantic authority.

---

## Motivation

C is fast, portable, and honest, but its native semantics are low-level and
mechanical. Writing non-trivial programs in C often requires memorizing
patterns, rules, and boilerplate such as manual memory management,
ownership conventions, error flags, and loop idioms. These details obscure
intent and slow development.

Many modern languages solve this by embedding abstractions directly into
the language. While powerful, this approach increases semantic complexity,
hides behavior, and raises the cost of mastery.

This project takes a different path.

C is left untouched.  
Meaning is added through **libraries**, not syntax.

The goal is to enable **literate, intention-revealing C code** while
preserving C’s performance, portability, and transparency.

Readability is treated as a real engineering constraint, directly
proportional to development time and long-term maintainability.

---

## Core Idea

Everything is a library.  
Everything is a function.  
Everything is explicit.

C is used as a **low-level target**, while higher-level semantics
(memory, lifetime, failure, data flow, transformation) are expressed
through small, composable modules.

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

## What This Is

- A curated set of **foundational C modules**
- A shared structure and discipline for community-defined function libraries
- A functional-style semantic layer built *on top of* C
- Fully usable from both **C and C++**
- A project optimized for readability, predictability, and development speed

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

---

## Project Structure

The project is organized by **semantic depth**, not by features.
Lower layers define unavoidable mechanics, while higher layers build
meaning on top of them.

