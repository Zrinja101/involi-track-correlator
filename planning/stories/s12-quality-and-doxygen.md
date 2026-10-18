# S12 — Static Quality and Doxygen

**Parent feature:** [F6 — Quality, Documentation, and Reproducibility](../features/f6-quality-documentation-and-reproducibility.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** In progress; Doxygen blocked in current environment

## Story

As a maintainer, I need code that follows the project’s C++ conventions and explains its public interfaces and non-obvious algorithms.

## Acceptance criteria

- clang-format and clang-tidy pass with no unresolved diagnostics.
- Google C++ naming, brace, include, unit-suffix, and test-file conventions are followed.
- Public types/functions document purpose, parameters, results, invariants, and failure behavior where applicable.
- Doxygen generation succeeds and generated output is inspected.

## Progress record

- clang-format and clang-tidy pass for the implemented C++ sources and project headers.
- `Doxyfile` and an optional CMake `doxygen` target are now defined.
- Generation is blocked in the current environment because the `doxygen` executable is not installed; the configured target is therefore unavailable until Doxygen is installed.
