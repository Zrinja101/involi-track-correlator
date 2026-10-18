# F1 — Input and Report Model

**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Complete
**Stories:** [S1](../stories/s1-report-model-and-parser.md), [S2](../stories/s2-validation-boundary.md)

## Goal

Convert JSONL input into a typed, validated internal report representation without losing the original report index or arrival-time semantics.

## Requirements

- Preserve the zero-based input line/report index.
- Parse required fields `arr`, `rx`, `t`, `lat`, and `lon`.
- Parse optional `id` and `alt` fields when present.
- Reject malformed JSON and invalid field types at a defined boundary.
- Detect non-finite numeric values and invalid geographic ranges.
- Preserve enough information for downstream timing, association, rejection, and output.
- Keep parsing independent of track-association policy.

## Acceptance criteria

- Valid sample reports produce deterministic typed values.
- Missing optional fields are represented explicitly, not with sentinel strings.
- Invalid JSON, missing required fields, wrong types, NaN/Inf, and impossible coordinates follow the documented error policy.
- Parser tests cover every optional-field combination and boundary value.
- The model’s fields and invariants are documented with Doxygen.

## Design notes

The parser should not decide which aircraft a report belongs to. It should establish syntactic and basic physical validity, leaving association confidence and track rejection to F4.

## Completion record

- **Completed stories:** S1, S2.
- **Result:** Typed parsing and basic report validation are implemented and independently tested.
- **Verification:** S1 and S2 completion records contain detailed test evidence and commit references: `2f41321`, `fe8c2ca`.
