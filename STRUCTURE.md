# Project Structure

This document defines the **architecture and dependency rules** for all modules
in the `Canon-C` ecosystem. It is intended to maintain clarity, composability, and
prevent accidental coupling.

---

## Semantic Layers

Modules are organized by **semantic depth**, not by feature count. Lower layers
define unavoidable mechanics; higher layers build meaning on top of them.


core/ — memory, lifetime, scope

semantics/ — meaning (option, result)

data/ — data shapes (vec, slice, range)

algo/ — transformations (map, filter, fold)

util/ — optional helpers



### Dependency Rule (Strict)

core → semantics → data → algo → util


- Lower layers may be used by higher layers.  
- Upward or circular dependencies are strictly forbidden.  
- Each module must be independently usable.  

This rule ensures **explicitness** and prevents hidden behaviors or fragile dependencies.

---

## Example

- `vec.h` in `data/` can depend on `core/arena.h` but **not** on `algo/map.h`.  
- `algo/map.h` can depend on `data/vec.h` but **not** on `util/io.h`.  
- `util/io.h` can depend on anything below it, but nothing above.
