# ADR-001: Deterministic Bounded Streaming Correlator

**Status:** Proposed  
**Parent epic:** [Production Track Correlator](../epic.md)

## Context

The input is an unbounded JSONL stream with monotonically increasing arrival times. Receiver timestamps are not synchronized, delivery can be delayed, reports can be incomplete or corrupted, and decisions must be emitted within two seconds of each report’s arrival.

## Decision proposal

Use a deterministic, modular streaming architecture with these conceptual boundaries:

1. **Report parser/model** — JSON decoding and basic field validation.
2. **Clock/timing service** — arrival-clock progression, receiver timing evidence, pending decision deadlines.
3. **Track store** — bounded active-track state, lifecycle, history retention, and unique track naming.
4. **Association engine** — candidate generation, evidence scoring, ambiguity handling, and state updates.
5. **Decision scheduler** — deadline-aware finalization and end-of-input flush.
6. **Output writer** — schema validation and JSONL serialization.

The exact mathematical association model remains an implementation decision to be selected and tested during story design. It must combine time and geographic consistency with cautious identifier use rather than assuming identifiers are globally unique.

## Invariants

- Every input report reaches exactly one terminal decision: assigned track or rejection.
- A finalized decision is never revised.
- Finalized `at` values are nondecreasing and within the report’s allowed interval.
- Rejected reports never mutate active track state.
- Active and pending state has explicit bounded retention.
- Track names are unique for the process lifetime.

## Alternatives considered

- **Immediate greedy assignment:** simple but vulnerable to delayed data and close crossings.
- **Unbounded global history:** potentially more accurate but violates the memory contract.
- **Identifier-only matching:** fails for missing, changing, and duplicate identifiers.
- **Probabilistic/ML model:** unnecessary external complexity and difficult deterministic verification.

## Consequences

The modular design creates test seams for each contract boundary and permits deterministic replay. It requires explicit policies for deadline finalization, stale-track eviction, confidence thresholds, and receiver-clock estimation.

## Open questions

- Which geographic prediction model provides sufficient accuracy without external dependencies?
- What retention window and active-track cap satisfy the scorer while guaranteeing bounded memory?
- How should ambiguous candidates be ranked at the exact decision deadline?
