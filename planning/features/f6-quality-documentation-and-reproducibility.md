# F6 — Quality, Documentation, and Reproducibility

**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Complete
**Stories:** [S11](../stories/s11-test-coverage-and-adversarial-fixtures.md), [S12](../stories/s12-quality-and-doxygen.md), [S13](../stories/s13-build-and-docker-reproducibility.md)

## Goal

Make correctness measurable and the finished project maintainable, reviewable, and reproducible.

## Requirements

- Use the existing pinned Catch2 test framework unless a later decision explicitly migrates to GoogleTest.
- Measure production line and branch coverage, documenting exclusions where applicable.
- Add adversarial fixtures beyond the provided sample.
- Enforce Google C++ style, project naming/unit conventions, clang-format, clang-tidy, and compiler diagnostics.
- Document public APIs, invariants, and non-obvious algorithms with Doxygen.
- Keep local and Docker builds reproducible and document all dependencies.

## Acceptance criteria

- All required verification commands pass and their evidence is recorded.
- Coverage reports record production-code coverage for continued quality improvement.
- Doxygen generation completes without unresolved documented API warnings.
- The unmodified Dockerfile builds and runs tests in an environment with Docker access.
- README instructions are sufficient for a new developer to build, test, score, and generate documentation.

## Progress record

- **Completed stories:** S11, S12, and S13 are complete.
- **Available evidence:** clang-format and clang-tidy have passed during S1–S10 implementation steps, the pinned dependency/build setup is recorded in `22d6374` and `bef9fa`, and F01 calibration passes.
- **Current calibration result:** F01–F08 and the holdout fixture pass their current functional checks; F04 has one expected retirement switch. F08 runtime is approximately 11.3 seconds for 10,000 reports. The historical sample baseline was 99.85% association; the current plausibility-integrated sample regression passes at 99.71% association, 18 ID switches, 1 merged track, 0 invalid accepted reports, and 0 contract errors.
- **Memory evidence:** F08 child-process peak RSS is 10,856 KB; container-level profiling remains outstanding.
- **Coverage progress:** 93.9% line, 100% function, and 59.6% branch coverage after additional parser, configuration, and lifecycle edge tests. Additional coverage work is non-blocking.
- **Documentation status:** Doxygen 1.9.6 generation passes and produces `build/docs/html/index.html`; public API documentation was added in `e83c62f`.
- **Build status:** Local Debug and Release configure/build/test paths pass with `-j10`.
- **Container status:** Docker verification passed on a separate Rancher Desktop-connected machine using the unmodified Dockerfile.
- **Final review:** Complete; no blocking quality or reproducibility issues remain. Container-level memory profiling and additional coverage work are optional follow-on work.
- **Verification plan:** [Verification Plan](../verification.md).
