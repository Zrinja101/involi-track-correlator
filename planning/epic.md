# Epic: Production Track Correlator

**Status:** In progress
**Repository:** `involi-track-correlator`  
**Parent:** None  
**Children:** [Features](#features)

## Objective

Replace the placeholder correlator with a production-quality C++20 streaming service that assigns each incoming aircraft report to a stable track, rejects reports that cannot be trusted, and obeys the JSONL input/output contract defined in [`README.md`](../README.md).

The correlator must make each decision within two seconds of report arrival, emit exactly one answer per report, preserve nondecreasing decision times, and use bounded memory for arbitrarily long input streams.

## Initial baseline

- `src/main.cpp` currently assigns every report a unique track.
- `tests/correlator_tests.cpp` contains one placeholder Catch2 test.
- Local GCC/G++ 14.2.1 and CMake 3.26.5 are compatible with the Dockerfile toolchain.
- Debug configure/build succeeds with Catch2 v3.7.1 fetched from a pinned archive.
- Baseline CTest: 1/1 placeholder test passes.
- Baseline sample score: 7,857 reports, 100% association, 553 ID switches, 0 merged tracks, 0 invalid accepted, 0 contract errors; overall `FAIL` because each report receives its own track.
- Docker verification is pending because Docker is not available inside the current development container.

## Implementation progress

### Completed stories

| Stories | Result | Commit references |
|---|---|---|
| S1–S2 | Typed report model and basic validation | `2f41321`, `fe8c2ca` |
| S3–S4 | Arrival-clock scheduling and bounded pending state | `e231cef`, `007137e` |
| S5–S7 | Track lifecycle, deterministic scoring, and identifier continuity | `8bd8183`, `c014e60`, `0787590` |
| S8 | Contextual motion plausibility validation | `4229347` |
| S9–S10 | JSONL contract and end-to-end association pipeline | `eefc2e0`, `0b6e3e5` |

### Current results

- Unit suite: 39 test cases and 148 assertions passed at the S10 milestone.
- CLI contract: passed.
- Scorer contract errors: 0.
- Sample metrics: 63.21% association, 2 ID switches, 4 merged tracks, and 1 invalid accepted report out of 15.
- Overall sample scorer: `FAIL`; the 99% association and merged-track requirements are not yet met.
- Remaining epic work: synthetic calibration fixtures, threshold/resource calibration, coverage measurement, Doxygen, performance/memory evidence, Docker verification, and final review.

### Completion-record policy

Every completed story and feature must contain a completion record with its result, verification evidence, and associated commit reference. This epic is updated at each milestone and remains `In progress` until the definition-of-done checklist is satisfied.

## Problem statement

Reports arrive from multiple receivers with unsynchronized clocks, variable network delay, missing or changing identifiers, occasional identifier collisions, close aircraft, periods of silence, and corrupted positions. A useful solution must combine identity, time, position, and track history without violating the strict streaming output contract.

## Scope

### In scope

- JSONL report parsing and validation.
- Receiver-clock and arrival-time handling.
- Delayed-report buffering and decision scheduling.
- Stable track creation, association, update, and retirement.
- Handling of optional, missing, changing, and duplicate identifiers.
- Geographic and temporal consistency checks.
- Rejection of corrupted or implausible reports.
- JSONL answer serialization and exact-once output.
- Unit, integration, contract, adversarial, performance, and memory tests.
- Google C++ style, clang-format, clang-tidy, Doxygen, and reproducible build documentation.

### Out of scope

- Network transport, persistence, distributed coordination, or external services.
- Real-time wall-clock scheduling; the input `arr` clock is authoritative.
- Machine-learning models or external map/airspace data.
- Changes to the scorer’s definition or thresholds.

## Contract and success criteria

The implementation must satisfy the README contract:

1. Parse one report per nonempty input line.
2. Answer every validly read report exactly once.
3. Allow answers in any order, but never emit an answer with `at` before the report’s `arr`.
4. Emit each answer with `arr <= at <= arr + 2`.
5. Never decrease `at` between output lines.
6. Keep memory bounded as input length increases.
7. Represent rejected reports with a JSON `null` track.
8. Meet or exceed the scorer’s default thresholds: at least 99% association, no more than one ID switch per aircraft, and no more than 25% invalid reports accepted.
9. Pass all unit, integration, contract, coverage, static-analysis, documentation, and reproducibility checks.

## Technical principles

- Prefer deterministic behavior and explicit invariants over hidden heuristics.
- Separate parsing, time management, association, track state, and output so each can be tested independently.
- Use bounded data structures and explicit eviction policies.
- Treat malformed data, NaN/Inf values, invalid coordinates, and numerical edge cases as first-class cases.
- Avoid treating an identifier as globally unique without corroborating evidence.
- Keep all physical quantities named with units where known.

## Accepted architecture decisions

The architecture decision record [ADR-001](decisions/adr-001-correlator-architecture.md) is accepted for implementation planning:

- Use a modular, deterministic, bounded streaming architecture.
- Use a hybrid staged association strategy: validate, gate candidates, combine deterministic evidence, score, and reject when ambiguity remains.
- Treat identifiers as supporting evidence rather than globally unique keys.
- Reject ambiguous, malformed, and incomplete reports with `track: null` while continuing processing when the input line index is known.
- Send diagnostics to stderr so stdout remains machine-readable JSONL.
- Do not update track state from rejected reports.

These decisions were selected to balance track quality, explainability, deterministic replay, bounded resources, and testability. Probabilistic/ML association and identifier-only matching are explicitly deferred or rejected because they add complexity or fail known input conditions.

## Risks and mitigations

| Risk | Mitigation |
|---|---|
| Ambiguous association during close approaches | Use temporal/geographic prediction, confidence scoring, and conservative rejection; add crossing scenarios. |
| Delayed reports violate the answer deadline | Model the `arr` clock explicitly and test the scheduling boundary. |
| Receiver clock offsets cause false separation | Estimate and maintain receiver-specific timing relationships from track evidence. |
| Unbounded history grows memory | Define retention windows and test memory behavior on long streams. |
| Sample overfitting | Build synthetic adversarial fixtures for every README-listed failure mode. |
| Dependency/network failures | Use pinned archive dependencies and verify both local and Docker builds. |

## Features

- [F1 — Input and report model](features/f1-input-and-report-model.md)
- [F2 — Time, buffering, and scheduling](features/f2-time-buffering-and-scheduling.md)
- [F3 — Track association and lifecycle](features/f3-track-association-and-lifecycle.md)
- [F4 — Validation and rejection](features/f4-validation-and-rejection.md)
- [F5 — Output and streaming contract](features/f5-output-and-streaming-contract.md)
- [F6 — Quality, documentation, and reproducibility](features/f6-quality-documentation-and-reproducibility.md)

## Definition of done

- [ ] All linked features and stories are complete or explicitly deferred.
- [ ] Source, requirements, tests, and verification evidence are traceable.
- [ ] Production code satisfies the input/output, timing, exact-once, and bounded-memory contract.
- [ ] Unit tests cover normal and edge-case behavior using Catch2.
- [ ] Production code reaches 100% measured line and branch coverage, with exclusions documented.
- [ ] Integration, scorer, regression, performance/memory, and Docker checks pass.
- [ ] Google C++ style, project naming conventions, clang-format, clang-tidy, and compiler diagnostics pass.
- [ ] Public APIs and non-obvious algorithms have professional Doxygen documentation.
- [ ] README and planning verification instructions are current.
- [ ] Final review identifies no blocking correctness, safety, resource, or maintainability issues.

## Navigation

- [Verification plan](verification.md)
- [Architecture decision record](decisions/adr-001-correlator-architecture.md)
- [Association model and resource bounds](decisions/adr-002-association-model-and-bounds.md)
- [Calibration and synthetic fixture strategy](decisions/adr-003-calibration-and-fixture-strategy.md)
