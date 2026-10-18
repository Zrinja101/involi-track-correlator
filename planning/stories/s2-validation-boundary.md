# S2 — Validation Boundary

**Parent features:** [F1](../features/f1-input-and-report-model.md), [F4](../features/f4-validation-and-rejection.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need a clear distinction between malformed input, physically invalid data, and valid-but-ambiguous observations so each case follows a predictable policy.

## Acceptance criteria

- Latitude and longitude domain boundaries are tested.
- NaN, infinity, missing required values, invalid altitude, and impossible timestamps are covered.
- Parse failures and rejected observations cannot corrupt scheduler or track state.
- Every input line still receives the required terminal answer where the application contract permits processing.

## Traceability

- Source scope: report validation and error types.
- Tests: validation boundary and output-contract tests.
- Verification: malformed-input, corruption, and exact-once fixtures.
