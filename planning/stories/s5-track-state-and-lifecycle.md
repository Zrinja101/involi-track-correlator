# S5 — Track State and Lifecycle

**Parent feature:** [F3 — Track Association and Lifecycle](../features/f3-track-association-and-lifecycle.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

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

- Source scope: `src/track_store.h`, `src/track_store.cpp`, and the `correlator_core` CMake target.
- Tests: `tests/correlator_tests.cpp`, covering creation, updates, history eviction, retirement, name monotonicity, and active-track capacity.
- Verification: Debug configure/build, CTest, clang-format, and clang-tidy completed successfully.
- Defaults: 4,096 active tracks, 32 observations per history, and 60 seconds to retirement; all limits are constructor-configurable.
