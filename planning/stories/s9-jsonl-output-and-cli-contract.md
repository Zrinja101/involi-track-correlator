# S9 — JSONL Output and CLI Contract

**Parent feature:** [F5 — Output and Streaming Contract](../features/f5-output-and-streaming-contract.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Implemented

## Story

As a pipeline user, I need the executable to consume stdin JSONL and produce only valid answer JSONL on stdout.

## Acceptance criteria

- Output contains exactly `report`, `track`, and `at` with correct types.
- Rejected reports use JSON `null`.
- Empty input, one report, EOF, and malformed-input behavior are defined and tested.
- Diagnostics never corrupt stdout answers.

## Traceability

- Source scope: `src/answer_writer.h`, `src/answer_writer.cpp`, `src/main.cpp`, and the integrated core pipeline.
- Tests: `tests/correlator_tests.cpp` for serialization and `tests/cli_contract_test.py` for executable stdin/stdout behavior.
- Verification: Debug configure/build, CTest, clang-format, clang-tidy, and executable JSONL contract checks completed successfully.
- Association remains intentionally unimplemented at this boundary; valid reports currently produce `track: null` until the association integration stories are complete.

## Completion record

- **Result:** Implemented JSONL serialization, exact-once output, monotonic decision times, EOF flushing, stderr diagnostics, and executable contract coverage.
- **Verification:** 39 test cases and 148 assertions passed at completion; CLI contract test passed; Debug build, CTest, clang-format, and clang-tidy passed.
- **Commit:** `eefc2e0` — `feat(S9): integrate JSONL output contract`
