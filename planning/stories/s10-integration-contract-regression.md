# S10 — Integration Contract Regression

**Parent feature:** [F5 — Output and Streaming Contract](../features/f5-output-and-streaming-contract.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

## Story

As a maintainer, I need executable-level regression tests that verify the complete parser, scheduler, association, and writer pipeline.

## Acceptance criteria

- Tests launch the built executable with representative JSONL stdin.
- The scorer reports zero contract errors for all integration fixtures.
- Sample scoring is recorded as a regression result.
- Output remains deterministic across repeated runs.

## Traceability

- Source scope: `src/main.cpp`, `src/track_store.cpp`, and the integrated parser, scheduler, plausibility, association, and writer components.
- Tests: `tests/cli_contract_test.py` and the full Catch2 unit suite.
- Verification: Debug configure/build, CTest, executable contract test, and sample scorer baseline completed.
- The S9 baseline scored 0% association with zero assigned tracks; this story establishes the first end-to-end assignment path.
