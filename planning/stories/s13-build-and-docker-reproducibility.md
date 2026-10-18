# S13 — Build and Docker Reproducibility

**Parent feature:** [F6 — Quality, Documentation, and Reproducibility](../features/f6-quality-documentation-and-reproducibility.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** In progress; Docker blocked in current environment

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
- Docker verification is blocked because the current development container has no `docker` executable or mounted Rancher Desktop socket. Rancher Desktop works from the integrated Ubuntu WSL environment, but that environment does not contain this `/workspaces` checkout.
