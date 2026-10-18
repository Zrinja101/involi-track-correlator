# S9 — JSONL Output and CLI Contract

**Parent feature:** [F5 — Output and Streaming Contract](../features/f5-output-and-streaming-contract.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As a pipeline user, I need the executable to consume stdin JSONL and produce only valid answer JSONL on stdout.

## Acceptance criteria

- Output contains exactly `report`, `track`, and `at` with correct types.
- Rejected reports use JSON `null`.
- Empty input, one report, EOF, and malformed-input behavior are defined and tested.
- Diagnostics never corrupt stdout answers.
