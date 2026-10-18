# ADR-003: Calibration and Synthetic Fixture Strategy

**Status:** In progress
**Parent epic:** [Production Track Correlator](../epic.md)
**Related design:** [ADR-002](adr-002-association-model-and-bounds.md)

## Purpose

Define how association parameters and resource limits will be selected without overfitting to `data/sample`. The correlator implementation must be calibrated against deterministic synthetic scenarios with known aircraft identity and expected outcomes.

## Fixture principles

- Generate fixtures deterministically from fixed seeds or explicit JSONL data.
- Keep reports and truth files together so every expected association is auditable.
- Vary one source of difficulty at a time before combining multiple difficulties.
- Include both positive assignments and intentional null-track rejections.
- Test exact boundaries and just-inside/just-outside values.
- Keep a holdout fixture set that is not used to choose weights or thresholds.
- Record the parameter set, fixture version, and scorer output with every calibration run.

## Required fixture families

### F01 — Single aircraft baseline

One aircraft, one receiver, regular observations, stable identifier, and smooth motion. Establishes the minimum viable track behavior and verifies that the model does not reject straightforward reports.

### F02 — Multiple receivers and clock offsets

One aircraft observed by multiple receivers with fixed clock offsets and variable delivery delay. Verifies receiver identity handling, offset estimation, and that `arr - t` is not incorrectly treated as pure clock offset.

### F03 — Missing, changing, and duplicate identifiers

- A track with no identifier throughout.
- An identifier appearing after a track is established.
- An aircraft changing identifier.
- Two aircraft broadcasting the same identifier.

Verifies that identifiers support association but do not override motion and position evidence.

### F04 — Gaps and retirement

An aircraft disappears for increasing intervals, then reappears near or beyond the configured retention boundary. Verifies prediction uncertainty, stale retirement, safe recreation, and bounded history.

### F05 — Close approaches and crossings

Two aircraft approach, cross, and separate with similar positions and optionally identical identifiers. Verifies candidate gating, score margins, conflict assignment, deterministic tie-breaking, and conservative ambiguity rejection.

### F06 — Corruption and implausible motion

Inject NaN/Inf where representable, out-of-range coordinates, isolated large jumps, inconsistent altitude, and malformed JSON or missing required fields. Verifies validation, null-track output, stderr diagnostics, and no track-state mutation.

### F07 — Delayed bursts and deadline boundaries

Deliver reports in bursts, vary receiver timestamps independently from arrival time, and place decisions exactly at `arr` and `arr + 2`. Verifies scheduler behavior, pending retention, monotonic `at`, and end-of-input flushing.

### F08 — Resource stress

Generate long streams with many aircraft, stale tracks, pending reports, and conflict components. Verify explicit maximum state, deterministic eviction, bounded conflict work, and acceptable runtime.

## Calibration procedure

1. Establish a simple baseline with no optional evidence and record failures.
2. Choose physically meaningful candidate gates from fixture tolerances and observed noise.
3. Add evidence weights and penalties one family at a time.
4. Select the smallest score margin and minimum score that prevent known false merges and swaps while meeting valid-report association targets.
5. Select history, inactivity, active-track, pending, and conflict-component bounds that pass F01–F08 and preserve hard memory limits.
6. Run the holdout fixtures without retuning.
7. Run the provided sample and record association, switches, merges, invalid acceptance, contract errors, runtime, and peak state.
8. Freeze the selected parameter set in documented constants or configuration and add boundary tests for every selected limit.

## Acceptance criteria

- Every parameter has a documented purpose, unit, default, and calibration fixture.
- No parameter is selected using only the provided sample.
- Holdout fixtures pass without parameter changes.
- Exact boundary behavior is tested for every gate, timeout, cap, and margin.
- The test harness can reproduce calibration results from a clean build.
- Calibration evidence is linked from the relevant stories and verification record.

## Calibration milestone: F01

- **Fixture:** `data/synthetic/f01-single-aircraft/`.
- **Generator:** `tools/generate_calibration_fixtures.py`.
- **Scenario:** 10 smooth observations for one aircraft, alternating receivers, stable identifier, and deterministic receiver-clock offsets.
- **Result:** `PASS`; 100.00% association, 0 ID switches, 0 merged tracks, 0 invalid accepted reports, and 0 contract errors.
- **Interpretation:** The current pipeline handles the uncomplicated single-aircraft baseline. This result does not calibrate or validate close-aircraft, identifier-collision, corruption, delay, or resource behavior.

## Open items

- Add fixture families F02–F08 and holdout coverage.
- Select final parameter values after the complete fixture set exists.
- Define peak-state and runtime measurements for F08.
