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

inline constexpr double kDefaultMaxAssociationAgeSecs = 60.0;
inline constexpr double kDefaultMaxAssociationDistanceM = 10000.0;
inline constexpr double kDefaultMinAssociationScore = 0.5;
inline constexpr double kDefaultMinAssociationMargin = 0.05;

struct AssociationConfig {
  double max_track_age_secs = kDefaultMaxAssociationAgeSecs;
  double max_distance_m = kDefaultMaxAssociationDistanceM;
  double min_score = kDefaultMinAssociationScore;
  double min_score_margin = kDefaultMinAssociationMargin;
  double distance_weight = 0.6;
  double age_weight = 0.2;
  double identifier_weight = 0.2;
};

struct AssociationCandidate {
  std::string track_id;
  double score;
  double distance_m;
};

struct AssociationResult {
  std::optional<std::string> assigned_track_id;
  std::vector<AssociationCandidate> candidates;
};

// Ranks feasible tracks using deterministic time, position, and identity
// evidence.
class AssociationScorer {
 public:
  explicit AssociationScorer(AssociationConfig config = {});

  // Returns candidates in deterministic score order and an optional assignment.
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
