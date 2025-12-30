CANON-C defines semantics, not implementations.

Including a header selects:
- ownership rules
- lifetime rules
- failure semantics
- allocation visibility

No CANON-C header:
- allocates implicitly
- hides failure
- obscures ownership

CANON-C treats C as an execution backend.
Meaning is chosen explicitly through headers.
