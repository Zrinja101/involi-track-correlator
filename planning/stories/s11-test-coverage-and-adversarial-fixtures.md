# S11 — Test Coverage and Adversarial Fixtures

**Parent feature:** [F6 — Quality, Documentation, and Reproducibility](../features/f6-quality-documentation-and-reproducibility.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** In progress

## Story

As a maintainer, I need comprehensive automated tests and measurable coverage so correctness does not depend on the sample dataset.

Synthetic fixture families, calibration procedure, and holdout policy must follow [ADR-003](../decisions/adr-003-calibration-and-fixture-strategy.md).

## Acceptance criteria

- Catch2 tests cover normal paths, error paths, boundary values, and README-listed failure modes.
- Production code reaches 100% line and branch coverage, with any exclusions documented.
- Fixtures cover delays, unsynchronized clocks, missing/changing/duplicate IDs, close aircraft, gaps, corruption, and long streams.
- Coverage commands are reproducible and documented.

## Progress record

- **Completed fixture families:** F01 single-aircraft baseline; F02 multiple receivers and clock offsets; F03 identifier changes and duplicates; F04 gaps and retirement.
- **Evidence:** `tools/generate_calibration_fixtures.py` deterministically generates the checked-in fixture directories.
- **Results:** F01–F03 each score `PASS` with 100.00% association and zero switches; F04 scores `PASS` with 100.00% association and one expected retirement switch. All have zero merged tracks, invalid accepted reports, and contract errors.
- **F05 finding:** The initial crossing fixture failed at 50.00% with one merged track. The 500 m gate plus exact-tie-only ambiguity rejection now produces 100.00% association with no merges or switches. The sample dataset remains below target and is still an active calibration input.
- **F06 result:** The initial `1 / 2` invalid acceptance is fixed by explicit contradictory identity/position rejection; F06 now passes with zero invalid accepted reports.
- **F07 result:** Delayed-burst and deadline-boundary fixture passes with 100.00% association, zero switches, zero merges, zero invalid accepted reports, and zero contract errors.
- **Remaining:** Add F08, add holdout fixtures, measure coverage, and collect resource/performance evidence.
