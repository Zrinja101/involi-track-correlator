# S3 — Arrival Clock and Scheduler

**Parent feature:** [F2 — Time, Buffering, and Scheduling](../features/f2-time-buffering-and-scheduling.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need deterministic arrival-clock scheduling so every decision is made in the permitted two-second window without wall-clock dependence.

## Acceptance criteria

- Scheduler never emits before `arr` and never after `arr + 2`.
- `at` is nondecreasing across output lines.
- Reports can be finalized out of input order when the contract permits.
- Tests cover equal timestamps, large arrival gaps, close deadlines, and already-advanced output time.

## Traceability

- Source scope: scheduler/deadline modules.
- Tests: scheduler unit tests and executable contract tests.
- Verification: scorer contract validation.
