# S1 — Report Model and Parser

**Parent feature:** [F1 — Input and Report Model](../features/f1-input-and-report-model.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need each JSONL line converted into a typed report so downstream logic can reason about time, position, identity, and provenance safely.

## Scope

- Add a typed report model and parser boundary.
- Preserve report index, `arr`, `rx`, `t`, position, optional identifier, and optional altitude.
- Define parse errors without writing diagnostics to stdout.

## Acceptance criteria

- Valid JSON with all required fields parses deterministically.
- Optional `id` and `alt` are represented as absent/present values.
- Missing required fields, wrong types, malformed JSON, and non-finite numbers are handled by the documented policy.
- Tests cover empty strings, whitespace, extra fields, integer/floating numeric forms, and optional-field combinations.

## Traceability

- Source scope: `src/` parser/model modules; integration with `src/main.cpp`.
- Tests: parser unit tests in `tests/`.
- Verification: F1 and F5 contract checks.
