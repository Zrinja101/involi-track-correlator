# S4 — Delayed Report Buffering

**Parent feature:** [F2 — Time, Buffering, and Scheduling](../features/f2-time-buffering-and-scheduling.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need bounded pending state so delayed receiver reports can improve association without allowing memory or decision latency to grow without limit.

## Acceptance criteria

- Late receiver timestamps do not change the arrival-clock contract.
- Burst delivery is processed without unbounded buffering.
- Pending reports are finalized at their deadlines and all remaining reports are flushed at EOF.
- Long-stream tests demonstrate bounded state under documented assumptions.

## Traceability

- Source scope: pending-report queue and flush logic.
- Tests: delayed burst, deadline, EOF, and memory fixtures.
