# S13 — Build and Docker Reproducibility

**Parent feature:** [F6 — Quality, Documentation, and Reproducibility](../features/f6-quality-documentation-and-reproducibility.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Complete

## Story

As a user of the repository, I need documented local and containerized build paths that reproduce the tested executable.

## Acceptance criteria

- Local Debug and Release builds succeed with bounded parallelism.
- CTest passes in the configured build.
- The unmodified Dockerfile builds the image and runs tests.
- The containerized executable can process sample input and produce scorer-valid output.
- Dependencies are pinned and explained in the README.
- Docker verification evidence records the environment and commands.

## Progress record

- Local Debug and Release configure/build/test paths pass with `-j10`.
- Dependencies are pinned through CMake archive URLs and hashes.
- Docker verification was completed successfully on a separate Rancher Desktop-connected machine using the unmodified Dockerfile.
- The image build, container test execution, sample run, and scorer output matched the projected results. The current development container remains unable to run Docker directly, but that environment limitation does not block the repository verification evidence.

## Completion record

- **Result:** Local Debug/Release and remote Rancher Docker build/test paths are verified.
- **Commits:** `9656b2e` — `docs(S13): record local build and Docker blocker` (initial evidence); `4dbd0a6` — `docs(S13): record Docker verification completion`.
- **Verification plan:** [Verification Plan](../verification.md).
