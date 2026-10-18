#ifndef INVOLI_TRACK_CORRELATOR_SRC_ASSOCIATION_SCORER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_ASSOCIATION_SCORER_H_

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "report.h"
#include "track_store.h"

namespace correlator {

/// Maximum age of a track eligible for association, in seconds.
inline constexpr double kDefaultMaxAssociationAgeSecs = 60.0;
/// Maximum horizontal candidate distance, in metres.
inline constexpr double kDefaultMaxAssociationDistanceM = 500.0;
/// Minimum normalized score required for assignment.
inline constexpr double kDefaultMinAssociationScore = 0.5;
/// Minimum score margin; zero still rejects exact ties.
inline constexpr double kDefaultMinAssociationMargin = 0.0;

/// Tunable weights and gates used by AssociationScorer.
struct AssociationConfig {
  /// Maximum age of the latest track observation, in seconds.
  double max_track_age_secs = kDefaultMaxAssociationAgeSecs;
  /// Maximum predicted horizontal distance, in metres.
  double max_distance_m = kDefaultMaxAssociationDistanceM;
  /// Minimum total score for an assignment.
  double min_score = kDefaultMinAssociationScore;
  /// Minimum score difference; exact ties are always rejected.
  double min_score_margin = kDefaultMinAssociationMargin;
  /// Weight applied to geographic evidence.
  double distance_weight = 0.6;
  /// Weight applied to observation age evidence.
  double age_weight = 0.2;
  /// Weight applied to identifier evidence.
  double identifier_weight = 0.2;
};

/// One scored, feasible track candidate.
struct AssociationCandidate {
  /// Candidate track name.
  std::string track_id;
  /// Weighted normalized association score.
  double score;
  /// Predicted horizontal distance from the report, in metres.
  double distance_m;
};

/// Complete result of evaluating one report against active tracks.
struct AssociationResult {
  /// Assigned track, or nullopt when the report is rejected/ambiguous.
  std::optional<std::string> assigned_track_id;
  /// Feasible candidates sorted by descending score and then track name.
  std::vector<AssociationCandidate> candidates;
  /// True when identity evidence contradicts the only position-compatible
  /// track.
  bool rejected_for_identifier_conflict = false;
};

/// Ranks feasible tracks using deterministic time, position, and identity
/// evidence.
class AssociationScorer {
 public:
  /// Constructs a scorer using the supplied gates and evidence weights.
  explicit AssociationScorer(AssociationConfig config = {});

  /// Returns candidates in deterministic score order and an optional
  /// assignment.
  ///
  /// Reports with no feasible candidate, insufficient score, an exact tie, or
  /// contradictory identity/position evidence are rejected.
  [[nodiscard]] AssociationResult Evaluate(
      const Report& report, std::span<const TrackState> tracks) const;

 private:
  [[nodiscard]] double CalculateDistanceM(const Report& report,
                                          const TrackState& track,
                                          double age_secs) const;

  AssociationConfig config_;
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_ASSOCIATION_SCORER_H_
