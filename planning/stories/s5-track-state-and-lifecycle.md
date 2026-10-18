# S5 — Track State and Lifecycle

**Parent feature:** [F3 — Track Association and Lifecycle](../features/f3-track-association-and-lifecycle.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need bounded active-track state with stable names, updates, stale retirement, and safe recreation.

History retention, active-track limits, and retirement behavior must follow the resource-bound policy in [ADR-002](../decisions/adr-002-association-model-and-bounds.md).

## Acceptance criteria

- New tracks receive unique deterministic names.
- Accepted reports update the correct track state.
- Stale tracks retire according to a documented policy.
- Retired names are not reused while doing so could make output ambiguous.
- Track store state remains bounded in long-stream tests.

## Traceability

- Source scope: track state/store modules.
- Tests: lifecycle, naming, stale-gap, and memory tests.
