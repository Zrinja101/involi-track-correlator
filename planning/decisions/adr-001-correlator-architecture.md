# ADR-001: Deterministic Bounded Streaming Correlator

**Status:** Accepted
**Parent epic:** [Production Track Correlator](../epic.md)

## Context

The input is an unbounded JSONL stream with monotonically increasing arrival times. Receiver timestamps are not synchronized, delivery can be delayed, reports can be incomplete or corrupted, and decisions must be emitted within two seconds of each report’s arrival.

## Decision

Use a deterministic, modular streaming architecture with these conceptual boundaries:

1. **Report parser/model** — JSON decoding and basic field validation.
2. **Clock/timing service** — arrival-clock progression, receiver timing evidence, pending decision deadlines.
3. **Track store** — bounded active-track state, lifecycle, history retention, and unique track naming.
4. **Association engine** — staged candidate generation, evidence scoring, ambiguity handling, and state updates.
5. **Decision scheduler** — deadline-aware finalization and end-of-input flush.
6. **Output writer** — schema validation and JSONL serialization.

The association engine will use a **hybrid staged strategy**:

1. Reject syntactically or physically invalid reports at the validation boundary.
2. Generate feasible candidate tracks using time, position, freshness, and receiver-clock constraints.
3. Treat identifier agreement as supporting evidence, not as proof of identity, because identifiers may be missing, changing, or duplicated.
4. Score feasible candidates with deterministic, explainable time/geographic/identity evidence.
5. Use a deterministic assignment/tie-breaking policy when multiple pending reports interact with the same candidate tracks.
6. Assign a track only when confidence is sufficient; otherwise emit `track: null` for the ambiguous report.

Malformed JSON and reports missing required fields will be rejected and processing will continue. The line index remains known from the input stream, so the application emits a null-track answer for that report; diagnostics go to stderr and never corrupt stdout JSONL. Rejected reports do not mutate track state.

## Invariants

- Every input report reaches exactly one terminal decision: assigned track or rejection.
- A finalized decision is never revised.
- Finalized `at` values are nondecreasing and within the report’s allowed interval.
- Rejected reports never mutate active track state.
- Active and pending state has explicit bounded retention.
- Track names are unique for the process lifetime.

## Rationale and alternatives considered

The hybrid staged strategy is preferred because it combines the explainability, determinism, bounded resource use, and testability of explicit heuristics with the ability to combine multiple weak signals. Hard candidate gates prevent impossible tracks from influencing the result, while staged scoring avoids treating any single identifier or threshold as authoritative.

- **Simple deterministic heuristic:** easy to test, but brittle around boundaries and vulnerable to greedy track swaps.
- **Hybrid staged scoring:** selected; preserves deterministic behavior while handling missing, changing, and duplicate identifiers and providing conservative ambiguity rejection.
- **Probabilistic/ML model:** not selected initially because it adds calibration, numerical, explainability, and coverage complexity without an available training set.
- **Immediate greedy assignment:** not selected as the sole strategy because delayed data and close crossings can cause swaps.
- **Unbounded global history:** rejected because it violates the memory contract.
- **Identifier-only matching:** rejected because identifiers may be missing, changing, or duplicated.

## Consequences

The modular design creates test seams for each contract boundary and permits deterministic replay. The selected strategy requires explicit policies for candidate gates, score normalization, ambiguity thresholds, deadline finalization, stale-track eviction, and receiver-clock estimation. Conservative rejection may lower recall in ambiguous cases, but it protects track integrity and the invalid-report metric.

## Follow-up design

The implementation-level geographic model, receiver-clock treatment, conflict assignment policy, and resource bounds are proposed in [ADR-002](adr-002-association-model-and-bounds.md). Numerical defaults remain subject to fixture calibration before implementation.
