# S8 — Corrupted and Implausible Reports

**Parent feature:** [F4 — Validation and Rejection](../features/f4-validation-and-rejection.md)  
**Parent epic:** [Production Track Correlator](../epic.md)  
**Status:** Ready for design

## Story

As the correlator, I need to reject corrupted or implausible observations rather than contaminate a valid aircraft track.

## Acceptance criteria

- Corrupted positions are rejected with `track: null` when they cannot be safely repaired.
- Implausible jumps and impossible motion are tested against the selected model.
- Rejected reports do not update track history or predictions.
- Valid boundary movement remains accepted.
- Invalid-acceptance rate meets the scorer threshold.
