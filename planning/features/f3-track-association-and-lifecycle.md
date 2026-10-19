# F3 — Track Association and Lifecycle

**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Complete
**Stories:** [S5](../stories/s5-track-state-and-lifecycle.md), [S6](../stories/s6-association-scoring.md), [S7](../stories/s7-identity-changes-and-collisions.md)

## Goal

Maintain stable track identities and associate reports using the hybrid staged model defined in [ADR-002](../decisions/adr-002-association-model-and-bounds.md): local geographic prediction, bounded track history, receiver-clock evidence, deterministic scoring, and conservative ambiguity rejection.

## Requirements

- Create stable, unique track names for newly observed aircraft.
- Associate reports with existing tracks using the accepted hybrid staged strategy: candidate gating followed by deterministic evidence scoring.
- Update track state after accepted reports.
- Retire stale tracks without allowing state to grow without bound.
- Support missing identifiers, changing identifiers, and duplicate identifiers.
- Avoid merging nearby aircraft solely because their positions or identifiers are similar.
- Reject ambiguous reports with `track: null` when evidence is insufficient at the decision deadline; do not mutate track state.

## Acceptance criteria

- Straight-line, multi-receiver, identifier-present, and identifier-absent scenarios remain on stable tracks.
- Identifier changes do not unnecessarily create a new track.
- Duplicate identifiers do not force two aircraft into one track.
- Close crossings and temporary gaps have deterministic expected outcomes.
- Track retirement and recreation do not reuse active names or corrupt historical output.
- Association decisions expose testable evidence/confidence rather than relying on untestable global state.

## Completion record

- **Completed stories:** S5, S6, S7.
- **Result:** Track state, deterministic scoring, identity history, identifier changes, and duplicate-ID handling are implemented.
- **Verification:** Completion records reference `8bd8183`, `c014e60`, and `0787590`. F05 scores 100.00% with no merges or switches after the S6 calibration fix. The historical sample baseline was 99.85% association; the current plausibility-integrated pipeline scores 99.71% association, 18 ID switches, 1 merged track, and zero invalid accepted reports.
