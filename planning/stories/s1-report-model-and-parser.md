# S1 — Report Model and Parser

**Parent feature:** [F1 — Input and Report Model](../features/f1-input-and-report-model.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

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

- Source scope: `src/report.h`, `src/report_parser.h`, `src/report_parser.cpp`, and the `correlator_core` CMake target.
- Tests: `tests/correlator_tests.cpp`, covering seven parser scenarios and 39 assertions.
- Verification: Debug configure/build, CTest, clang-format, and clang-tidy completed successfully.
- Deferred to S2: geographic range policy and application-level rejection/output behavior.

## Completion record

- **Result:** Implemented typed JSONL parsing with structured errors, optional-field preservation, finite-number checks, and report-index preservation.
- **Verification:** 8 test cases and 37 assertions passed at completion; Debug build, CTest, clang-format, and clang-tidy passed.
- **Commit:** `2f41321` — `feat(S1): add typed report parser`
