# F3 — Track Association and Lifecycle

**Parent epic:** [Production Track Correlator](../epic.md)  
**Stories:** [S5](../stories/s5-track-state-and-lifecycle.md), [S6](../stories/s6-association-scoring.md), [S7](../stories/s7-identity-changes-and-collisions.md)

## Goal

Maintain stable track identities and associate reports using evidence from identifiers, time, position, receiver history, and track predictions.

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
