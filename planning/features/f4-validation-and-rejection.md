# F4 — Validation and Rejection

**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented; integration hardening pending
**Stories:** [S2](../stories/s2-validation-boundary.md), [S8](../stories/s8-corrupted-and-implausible-reports.md)

## Goal

Prevent corrupted or physically implausible reports from contaminating real tracks while preserving the exact-once output contract.

## Requirements

- Distinguish syntactic invalidity from association uncertainty.
- Reject malformed JSON and missing required fields with a null-track answer when the input line index is known, then continue processing subsequent lines.
- Detect corrupted positions and non-finite values.
- Reject reports that cannot be assigned with sufficient confidence using `track: null`.
- Ensure rejected reports do not update track state.
- Diagnostics are written to stderr only; stdout remains valid JSONL answers.
- Keep rejection decisions within the same timing and ordering contract as accepted reports.
- Avoid rejecting valid reports merely because optional identifiers are absent.

## Acceptance criteria

- Corrupted sample-style reports are rejected or handled according to the documented policy.
- A rejected report cannot alter subsequent association decisions.
- Boundary geographic values are accepted when valid and rejected when outside the valid domain.
- Invalid, ambiguous, and stale cases have separate tests and documented rationale.

## Completion record

- **Completed stories:** S2, S8.
- **Result:** Basic report-domain validation and contextual motion plausibility checks are implemented. Track mutation protection is enforced in the S10 pipeline.
- **Verification:** Completion records reference `fe8c2ca` and `4229347`; S10 integration evidence is recorded in the S10 story. The F06 contradictory identity/position acceptance is fixed by the S7/S8 integration follow-up.
