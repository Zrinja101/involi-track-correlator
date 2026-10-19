# F5 — Output and Streaming Contract

**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Complete
**Stories:** [S9](../stories/s9-jsonl-output-and-cli-contract.md), [S10](../stories/s10-integration-contract-regression.md)

## Goal

Provide a robust stdin/stdout JSONL executable that emits exactly the required answer schema and remains usable in pipelines.

## Requirements

- Emit one JSON object per answer with `report`, `track`, and `at`.
- Use JSON `null` for rejected reports.
- Answer every input report exactly once.
- Permit output order to differ from input order while preserving monotonic `at`.
- Avoid diagnostics on stdout; diagnostics must not corrupt answer streams.
- Handle empty input and end-of-file cleanly.

## Acceptance criteria

- The scorer reports zero contract errors for all supported fixtures.
- Malformed output cannot be produced by normal error paths.
- Empty, single-report, multi-report, delayed, and end-of-input cases are covered.
- An executable integration test validates the actual stdin/stdout boundary rather than only internal functions.

## Completion record

- **Completed stories:** S9, S10.
- **Result:** The executable now produces deterministic JSONL answers, integrates the association pipeline, and satisfies the output contract with zero scorer contract errors.
- **Verification:** Completion records reference `eefc2e0` and `0b6e3e5`; the final sample regression passes the epic scorer thresholds.
