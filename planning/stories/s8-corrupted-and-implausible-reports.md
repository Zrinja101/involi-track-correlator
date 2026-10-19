# S8 — Corrupted and Implausible Reports

**Parent feature:** [F4 — Validation and Rejection](../features/f4-validation-and-rejection.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

## Story

As the correlator, I need to reject corrupted or implausible observations rather than contaminate a valid aircraft track.

## Acceptance criteria

- Corrupted positions are rejected with `track: null` when they cannot be safely repaired.
- Implausible jumps and impossible motion are tested against the selected model.
- Rejected reports do not update track history or predictions.
- Valid boundary movement remains accepted.
- Invalid-acceptance rate meets the scorer threshold.

## Traceability

- Source scope: `src/plausibility_validator.h`, `src/plausibility_validator.cpp`, and the `correlator_core` CMake target.
- Tests: `tests/correlator_tests.cpp`, covering realistic movement, horizontal jumps, vertical jumps, and duplicate-time observations.
- Verification: Debug configure/build, CTest, clang-format, and clang-tidy completed successfully.
- Verification plan: [Verification Plan](../verification.md).
- Initial speed limits are explicit configuration and remain subject to ADR-003 calibration. Integration with track mutation and null-track output is exercised by S10; F06 contradictory-evidence rejection is now enforced by the association pipeline.

## Completion record

- **Result:** Implemented contextual horizontal/vertical motion checks and non-increasing-time handling without mutating track state in the validator.
- **Verification:** 38 test cases and 142 assertions passed at completion; Debug build, CTest, clang-format, and clang-tidy passed.
- **Commit:** `4229347` — `feat(S8): reject implausible report motion`
- **Follow-up fix:** `343857b` — `fix(S8): reject contradictory identity position`; contradictory identity/position reports are rejected without creating a new track.
