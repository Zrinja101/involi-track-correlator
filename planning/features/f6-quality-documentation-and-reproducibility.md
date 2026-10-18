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

- **Completed stories:** None yet; S11 is in progress and S12–S13 remain pending.
- **Available evidence:** clang-format and clang-tidy have passed during S1–S10 implementation steps, the pinned dependency/build setup is recorded in `22d6374` and `bef9fa`, and F01 calibration passes.
- **Current calibration result:** F01–F08 and the holdout fixture pass their current functional checks; F04 has one expected retirement switch. F08 runtime is approximately 11.3 seconds for 10,000 reports, while the sample regression remains below target at 84.81% association, 121 switches, and 3 merged tracks. Peak RSS is not yet measured.
- **Coverage baseline:** 89.6% line, 100% function, and 53.9% branch coverage; the 100% line/branch target remains open.
- **Documentation status:** Doxyfile and optional target are configured, but generation is blocked because Doxygen is not installed.
- **Remaining work:** increase line/branch coverage, install/run Doxygen, collect peak-memory evidence, and verify Docker.
