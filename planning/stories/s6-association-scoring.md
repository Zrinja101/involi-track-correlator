# S6 — Association Scoring

**Parent feature:** [F3 — Track Association and Lifecycle](../features/f3-track-association-and-lifecycle.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need deterministic candidate scoring so reports are associated using explainable temporal, geographic, receiver, and identity evidence.

## Acceptance criteria

- Candidate generation excludes impossible or stale tracks.
- Scores handle missing optional evidence without accidental bias.
- Ties and low-confidence cases have deterministic outcomes.
- Straight-line, receiver-overlap, close-proximity, crossing, and gap fixtures are covered.
- Sample scoring meets the epic thresholds without overfitting to one fixture.

## Traceability

- Source scope: association engine and supporting geographic/time utilities.
- Tests: association unit tests and scorer regression fixtures.
