# ADR-002: Association Model and Resource Bounds

**Status:** Proposed
**Parent epic:** [Production Track Correlator](../epic.md)
**Supersedes open questions in:** [ADR-001](adr-001-correlator-architecture.md)

## Context

ADR-001 selected a deterministic hybrid staged association strategy but left the geographic model, receiver-clock treatment, retention policy, and conflict assignment method open. These choices must be explicit before implementation so behavior is deterministic, bounded, and testable.

The sample data is concentrated around a relatively small geographic region. Reports contain both correlator arrival time (`arr`) and receiver time (`t`), while receiver clocks and network delays are not synchronized.

## Proposed decision

### Geographic representation

Use a local tangent-plane approximation for short-range calculations:

- Select a reference latitude/longitude for each processing context.
- Convert longitude and latitude deltas to local east/north distances using an Earth-radius constant and the cosine of the reference latitude.
- Treat altitude as an optional vertical component when present; do not require it for association.
- Keep the conversion and inverse conversion in a small, independently tested utility.

This avoids external geographic dependencies and is sufficiently accurate for the local sample scale. The implementation must validate finite values and geographic bounds before conversion.

### Motion prediction

Represent each active track with its most recent accepted observation and, when enough history exists, a bounded constant-velocity estimate in local coordinates. Predict the track position at the report’s normalized observation time, using uncertainty that grows with elapsed time and limited history.

When insufficient history exists, use a position-only candidate gate with conservative uncertainty. Do not infer a velocity from a single observation.

### Receiver-clock evidence

Maintain a receiver-specific timing estimate only from accepted associations with sufficient evidence:

- Start without an offset estimate.
- Derive candidate offset observations from correlated reports, while treating delivery delay as noise rather than assuming `arr - t` is a pure clock offset.
- Update with a robust bounded estimator such as a median or trimmed estimate.
- Cap the influence of one observation and decay or invalidate stale estimates.
- Fall back to arrival-time and position evidence when the estimate is unavailable or low confidence.

Receiver-clock evidence is supporting evidence and must never override a strong geographic or temporal contradiction.

### Candidate scoring

Apply the hybrid stages in this order:

1. Parse and validate the report.
2. Generate candidates whose track age, predicted distance, and normalized time are feasible.
3. Compute deterministic normalized evidence for position, motion, time, receiver timing, identifier agreement, identifier conflict, and altitude where available.
4. Combine evidence with documented weights and penalties.
5. Require a minimum score and a minimum margin over the next candidate.
6. Assign only when the result is sufficiently confident; otherwise reject with `track: null`.

Weights, gates, margins, and uncertainty growth must be named constants or configuration values and calibrated against synthetic fixtures and the sample data. They must not be hidden literals in the association loop.

### Conflict assignment

When multiple pending reports compete for the same candidate tracks, partition the conflict into a bounded connected component:

- Resolve singleton components using the deterministic score.
- For small multi-report components, choose the maximum-total-score one-to-one assignment with deterministic tie-breaking.
- Reject assignments that fail the minimum score or ambiguity margin.
- If a component exceeds the configured safety bound, finalize conservatively rather than allowing unbounded computation or memory growth.

The component bound, tie-break order, and fallback behavior must be explicit and covered by tests.

### Retention and resource bounds

Use both time and count bounds:

- Pending reports are retained only until their two-second decision deadline, then finalized.
- Each active track retains only a recent history window and a maximum number of observations.
- Tracks are retired after a documented inactivity timeout.
- The active-track store and conflict components have explicit maximum sizes.
- Eviction and retirement are deterministic and observable in tests.

Initial numerical limits are implementation parameters to be selected by fixture calibration. They must be conservative enough for the scorer and finite enough to provide a hard memory bound.

### F05 gate calibration note

The default association distance gate was tightened from 10,000 m to 500 m after F05 exposed a same-identifier merge. The default ambiguity margin was then set to zero while retaining exact-tie rejection. Together these changes allow strictly better candidates through close crossings without merging or accepting exact ties. These remain calibration parameters, not a final freeze.

## Rationale

A local coordinate model is simpler and more transparent than introducing a geographic library for the sample’s scale. Constant-velocity prediction uses available history without requiring a complex dynamics model. Robust receiver timing estimates account for the fact that delivery delay contaminates direct arrival-time differences. A bounded conflict assignment prevents greedy swaps without creating an unbounded global optimization problem. Explicit time and count limits satisfy the streaming memory contract.

## Required verification

- Coordinate conversion round trips and boundary/finite-value cases.
- Constant-velocity prediction with one, two, and many observations.
- Receiver offsets with delay noise, missing estimates, stale estimates, and abrupt outliers.
- Candidate gates at exact boundaries and just outside them.
- Score normalization, identifier conflicts, missing identifiers, and tie margins.
- Singleton and multi-report conflict components, deterministic ties, oversized-component fallback.
- History eviction, track retirement, pending deadline flush, and long-stream memory behavior.
- Sample scoring plus synthetic crossings, close aircraft, gaps, delayed bursts, and corrupted positions.

## Consequences

The implementation remains dependency-light, deterministic, and independently testable. It introduces several calibrated parameters and requires careful fixture design to prevent overfitting. Conservative rejection may reduce association recall in genuinely ambiguous cases, but it protects stable tracks and the invalid-report metric.

## Open items before implementation

Parameter selection and holdout-fixture policy are defined in [ADR-003](adr-003-calibration-and-fixture-strategy.md).

- Select numerical defaults for gates, weights, margins, uncertainty growth, retention windows, and component bounds from tests.
- Confirm whether altitude should contribute to the initial score or remain a later enhancement.
- Define the exact deterministic tie-break order and evidence logging/test seam.
