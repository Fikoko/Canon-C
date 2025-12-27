# Canon-C

A community-curated collection of small, composable C libraries that provide **explicit, selectable semantics** for writing readable, literate, and predictable C code.

This project treats **C as an execution backend**, not as a complete semantic system.  
Meaning, structure, and programming style are expressed through libraries — not imposed by the language.

---

## Motivation

Modern programming languages often trade readability and semantic transparency for expressive power and enforcement. While this can be effective in large or highly specialized environments, it also increases cognitive load, development time, and the need for deep language mastery.

This project explores a different path:

- prioritize **literate readability**
- make **semantics explicit and inspectable**
- reduce accidental complexity
- avoid hidden behavior
- keep performance and predictability

C is used because it is simple, stable, fast, and universally supported — not because its default semantics are ideal.

---

## Core Idea

**All abstractions live in libraries.  
All meaning is opt-in.  
Everything is replaceable.**

Instead of a single framework or a fixed “standard way”, this repository collects **independent function sets**, each representing a specific semantic choice.

Users select the semantics they want by selecting the libraries they include.

---

## What This Repository Contains

- Small, focused C libraries
- Each library is:
  - standalone
  - optional
  - documented
  - versioned by semantic intent
- Examples of library categories:
  - memory models (e.g. arena allocators)
  - semantic containers (e.g. vectors, options, results)
  - functional-style utilities (map, filter, fold)
  - iteration and data-flow helpers

There is no required core and no mandatory dependency graph.

---

## Community-Driven Semantics

This project is intentionally **pluralistic**.

Different libraries may solve the same problem using different assumptions or trade-offs.  
This is not a flaw — it is the point.

Two vector implementations can coexist.
Two error-handling models can coexist.
Two memory models can coexist.

Users choose **which meaning they want**, not which framework they must accept.

---

## What This Is Not

- Not a framework
- Not a new language
- Not a replacement for C++
- Not an STL clone
- Not a package manager

This project does not aim to compete with existing languages or ecosystems.  
It aims to make **writing and reasoning about C code simpler and more readable**.

---

## Design Principles

- **Library, not language**
- **Functions over mechanisms**
- **Explicit memory and lifetimes**
- **No hidden allocation**
- **Minimal surface area**
- **Readable before clever**
- **Literate programming friendly**
- **C and C++ compatible**

---

## Usage Philosophy

You do not “adopt” this project.

You **pick** from it.

Copy headers.
Use submodules.
Select only what fits your way of thinking.

If a library no longer matches your semantics, replace it.

---

## Philosophy

> Abstractions should be chosen, not inherited.  
> Semantics should be visible, not implied.  
> C provides execution — libraries provide meaning.

---

## Status

Early-stage and exploratory.

The focus is on clarity, restraint, and long-term usefulness rather than completeness or popularity.
