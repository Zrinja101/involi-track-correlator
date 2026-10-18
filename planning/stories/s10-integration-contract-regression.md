# S10 — Integration Contract Regression

**Parent feature:** [F5 — Output and Streaming Contract](../features/f5-output-and-streaming-contract.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As a maintainer, I need executable-level regression tests that verify the complete parser, scheduler, association, and writer pipeline.

## Acceptance criteria

- Tests launch the built executable with representative JSONL stdin.
- The scorer reports zero contract errors for all integration fixtures.
- Sample scoring is recorded as a regression result.
- Output remains deterministic across repeated runs.
