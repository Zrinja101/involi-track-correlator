# S6 — Association Scoring

**Parent feature:** [F3 — Track Association and Lifecycle](../features/f3-track-association-and-lifecycle.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

## Story

As the correlator, I need deterministic candidate scoring so reports are associated using explainable temporal, geographic, receiver, and identity evidence.

The implementation must follow [ADR-002](../decisions/adr-002-association-model-and-bounds.md), including local-coordinate prediction, candidate gates, normalized evidence, minimum-score and margin checks, and bounded conflict assignment.

## Acceptance criteria

- Candidate generation excludes impossible or stale tracks.
- Scores handle missing optional evidence without accidental bias.
- Ties and low-confidence cases have deterministic outcomes.
- Straight-line, receiver-overlap, close-proximity, crossing, and gap fixtures are covered.
- Sample scoring meets the epic thresholds without overfitting to one fixture.

## Traceability

- Source scope: `src/association_scorer.h`, `src/association_scorer.cpp`, and the `correlator_core` CMake target.
- Tests: `tests/correlator_tests.cpp`, covering distance ranking, identifier evidence, missing identifiers, stale/distant gates, and ambiguity rejection.
- Verification: Debug configure/build, CTest, clang-format, and clang-tidy completed successfully.
- Initial scoring weights and gates are explicit constructor configuration and remain subject to ADR-003 calibration.

## Completion record

- **Result:** Implemented deterministic local-coordinate distance scoring, constant-velocity prediction from history, age/distance gates, identifier evidence, minimum-score checks, and ambiguity margins.
- **Verification:** 30 test cases and 127 assertions passed at completion; Debug build, CTest, clang-format, and clang-tidy passed.
- **Commit:** `c014e60` — `feat(S6): add deterministic association scoring`
- **Calibration follow-up:** The default distance gate is now 500 m; this is recorded in a separate calibration-fix commit after F05 reduced merging but still left ambiguity rejections.
