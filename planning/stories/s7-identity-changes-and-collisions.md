# S7 — Identity Changes and Collisions

**Parent feature:** [F3 — Track Association and Lifecycle](../features/f3-track-association-and-lifecycle.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need identifier evidence to be useful without treating identifiers as permanently unique.

## Acceptance criteria

- Reports with no identifier can remain associated by other evidence.
- An aircraft changing identifier can retain its track when motion supports continuity.
- Two aircraft sharing an identifier are not forced into one track.
- Identifier evidence cannot override strong contradictory position/time evidence.
- Each behavior has isolated unit fixtures and multi-report integration fixtures.
