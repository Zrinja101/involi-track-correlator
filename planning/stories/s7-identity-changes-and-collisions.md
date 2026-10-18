# S7 — Identity Changes and Collisions

**Parent feature:** [F3 — Track Association and Lifecycle](../features/f3-track-association-and-lifecycle.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

## Story

As the correlator, I need identifier evidence to be useful without treating identifiers as permanently unique.

## Acceptance criteria

- Reports with no identifier can remain associated by other evidence.
- An aircraft changing identifier can retain its track when motion supports continuity.
- Two aircraft sharing an identifier are not forced into one track.
- Identifier evidence cannot override strong contradictory position/time evidence.
- Each behavior has isolated unit fixtures and multi-report integration fixtures.

## Traceability

- Source scope: `src/identifier_matcher.h`, `src/identifier_matcher.cpp`, and the history-aware identifier scoring path in `src/association_scorer.cpp`.
- Tests: `tests/correlator_tests.cpp`, covering historical identifier matches, identifier changes, duplicate identifiers, missing identifiers, and conflicts.
- Verification: Debug configure/build, CTest, clang-format, and clang-tidy completed successfully.
- Identifier history is bounded by the S5 track-history limit; final weights remain subject to ADR-003 calibration.

## Completion record

- **Result:** Implemented history-aware identifier matching so identifier changes preserve continuity, duplicate identifiers remain position-resolved, and missing/conflicting identifiers remain distinguishable.
- **Verification:** 34 test cases and 134 assertions passed at completion; Debug build, CTest, clang-format, and clang-tidy passed.
- **Commit:** `0787590` — `feat(S7): handle changing and duplicate identifiers`
- **Follow-up:** `343857b` adds explicit rejection when an identifier matches an existing track but position only fits a conflicting track.
