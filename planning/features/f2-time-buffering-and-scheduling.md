# F2 — Time, Buffering, and Scheduling

**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Complete
**Stories:** [S3](../stories/s3-arrival-clock-and-scheduler.md), [S4](../stories/s4-delayed-report-buffering.md)

## Goal

Make decisions using the input arrival clock while allowing enough bounded delay to correlate reports without violating the two-second answer window.

## Requirements

- Treat `arr` as the only decision clock; wall-clock speed is irrelevant.
- Never schedule an answer before the report’s arrival.
- Answer every report by `arr + 2`.
- Keep emitted `at` values nondecreasing.
- Handle late receiver timestamps and bursts caused by network stalls.
- Flush pending reports deterministically at end of input.
- Bound pending state and define behavior when late data cannot be retained.

## Acceptance criteria

- Boundary tests pass at exactly `arr`, exactly `arr + 2`, and just outside the allowed interval.
- Output ordering and monotonic `at` behavior hold when reports are answered out of input order.
- Delayed bursts do not cause unbounded queues.
- End-of-input processing answers all remaining reports exactly once.
- Scheduler tests use a controllable arrival clock and do not depend on sleeping.

## Completion record

- **Completed stories:** S3, S4.
- **Result:** Arrival-clock deadlines, deterministic EOF behavior, and bounded pending capacity are implemented.
- **Verification:** Completion records reference `e231cef` and `007137e` with test and static-analysis results.
