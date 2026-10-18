# F6 — Quality, Documentation, and Reproducibility

**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Planned
**Stories:** [S11](../stories/s11-test-coverage-and-adversarial-fixtures.md), [S12](../stories/s12-quality-and-doxygen.md), [S13](../stories/s13-build-and-docker-reproducibility.md)

## Goal

Make correctness measurable and the finished project maintainable, reviewable, and reproducible.

## Requirements

- Use the existing pinned Catch2 test framework unless a later decision explicitly migrates to GoogleTest.
- Measure production line and branch coverage and reach 100%, documenting exclusions.
- Add adversarial fixtures beyond the provided sample.
- Enforce Google C++ style, project naming/unit conventions, clang-format, clang-tidy, and compiler diagnostics.
- Document public APIs, invariants, and non-obvious algorithms with Doxygen.
- Keep local and Docker builds reproducible and document all dependencies.

## Acceptance criteria

- All required verification commands pass and their evidence is recorded.
- Coverage reports show the agreed target for production code.
- Doxygen generation completes without unresolved documented API warnings.
- The unmodified Dockerfile builds and runs tests in an environment with Docker access.
- README instructions are sufficient for a new developer to build, test, score, and generate documentation.

## Progress record

- **Completed stories:** None yet; S11–S13 remain pending.
- **Available evidence:** clang-format and clang-tidy have passed during S1–S10 implementation steps, and the pinned dependency/build setup is recorded in `22d6374` and `bef9fa`.
- **Remaining work:** synthetic calibration fixtures, measured line/branch coverage, Doxygen generation, performance/memory evidence, and Docker verification.
