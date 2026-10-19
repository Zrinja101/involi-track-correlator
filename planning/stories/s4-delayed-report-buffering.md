# S4 — Delayed Report Buffering

**Parent feature:** [F2 — Time, Buffering, and Scheduling](../features/f2-time-buffering-and-scheduling.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

## Story

As the correlator, I need bounded pending state so delayed receiver reports can improve association without allowing memory or decision latency to grow without limit.

Pending deadlines and conflict-component bounds must follow [ADR-002](../decisions/adr-002-association-model-and-bounds.md).

## Acceptance criteria

- Late receiver timestamps do not change the arrival-clock contract.
- Burst delivery is processed without unbounded buffering.
- Pending reports are finalized at their deadlines and all remaining reports are flushed at EOF.
- Long-stream tests demonstrate bounded state under documented assumptions.

## Traceability

- Source scope: bounded pending state in `src/arrival_scheduler.h` and `src/arrival_scheduler.cpp`.
- Tests: `tests/correlator_tests.cpp`, including capacity-triggered finalization and delayed burst behavior.
- Verification: Debug configure/build, CTest, clang-format, and clang-tidy completed successfully.
- Verification plan: [Verification Plan](../verification.md).
- The default pending capacity is 4096 reports; tests use a small explicit capacity to exercise the bound.

## Completion record

- **Result:** Added configurable pending capacity and deterministic oldest-report early finalization at the current arrival clock when capacity is reached.
- **Verification:** 20 test cases and 89 assertions passed at completion; Debug build, CTest, clang-format, and clang-tidy passed.
- **Commit:** `007137e` — `feat(S4): bound pending report buffering`
