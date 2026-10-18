# F4 — Validation and Rejection

**Parent epic:** [Production Track Correlator](../epic.md)  
**Stories:** [S2](../stories/s2-validation-boundary.md), [S8](../stories/s8-corrupted-and-implausible-reports.md)

## Goal

Prevent corrupted or physically implausible reports from contaminating real tracks while preserving the exact-once output contract.

## Requirements

- Distinguish syntactic invalidity from association uncertainty.
- Detect corrupted positions and non-finite values.
- Reject reports that cannot be assigned with sufficient confidence using `track: null`.
- Ensure rejected reports do not update track state.
- Keep rejection decisions within the same timing and ordering contract as accepted reports.
- Avoid rejecting valid reports merely because optional identifiers are absent.

## Acceptance criteria

- Corrupted sample-style reports are rejected or handled according to the documented policy.
- A rejected report cannot alter subsequent association decisions.
- Boundary geographic values are accepted when valid and rejected when outside the valid domain.
- Invalid, ambiguous, and stale cases have separate tests and documented rationale.
