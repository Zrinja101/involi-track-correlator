#include "association_scorer.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "identifier_matcher.h"

namespace correlator {
namespace {

constexpr double kMetersPerDegree = 111320.0;
constexpr double kDegreesToRadians = std::numbers::pi / 180.0;

}  // namespace

AssociationScorer::AssociationScorer(AssociationConfig config)
    : config_(config) {}

AssociationResult AssociationScorer::Evaluate(
    const Report& report, std::span<const TrackState> tracks) const {
  AssociationResult result;
  for (const TrackState& track : tracks) {
    const double age_secs =
        report.arrival_time_secs - track.latest_report.arrival_time_secs;
    if (age_secs < 0.0 || age_secs > config_.max_track_age_secs) {
      continue;
    }

    const double distance_m = CalculateDistanceM(report, track, age_secs);
    if (distance_m > config_.max_distance_m) {
      continue;
    }

    const double distance_score = 1.0 - distance_m / config_.max_distance_m;
    const double age_score = 1.0 - age_secs / config_.max_track_age_secs;
    const IdentifierRelation identifier_relation =
        IdentifierMatcher::Compare(report, track);
    const double identifier_score =
        identifier_relation == IdentifierRelation::kMatch      ? 1.0
        : identifier_relation == IdentifierRelation::kConflict ? 0.0
                                                               : 0.5;
    const double score = config_.distance_weight * distance_score +
                         config_.age_weight * age_score +
                         config_.identifier_weight * identifier_score;
    result.candidates.push_back(AssociationCandidate{
        .track_id = track.track_id, .score = score, .distance_m = distance_m});
  }

  std::sort(result.candidates.begin(), result.candidates.end(),
            [](const AssociationCandidate& first,
               const AssociationCandidate& second) {
              if (first.score != second.score) {
                return first.score > second.score;
              }
              return first.track_id < second.track_id;
            });

  if (result.candidates.empty() ||
      result.candidates.front().score < config_.min_score) {
    return result;
  }
  if (result.candidates.size() > 1U &&
      result.candidates[0].score - result.candidates[1].score <=
          config_.min_score_margin) {
    return result;
  }
  result.assigned_track_id = result.candidates.front().track_id;
  return result;
}

double AssociationScorer::CalculateDistanceM(const Report& report,
                                             const TrackState& track,
                                             double age_secs) const {
  double predicted_latitude_degs = track.latest_report.latitude_degs;
  double predicted_longitude_degs = track.latest_report.longitude_degs;
  if (track.history.size() >= 2U) {
    const Report& previous_report = track.history[track.history.size() - 2U];
    const double interval_secs = track.latest_report.arrival_time_secs -
                                 previous_report.arrival_time_secs;
    if (interval_secs > 0.0) {
      predicted_latitude_degs +=
          (track.latest_report.latitude_degs - previous_report.latitude_degs) /
          interval_secs * age_secs;
      predicted_longitude_degs += (track.latest_report.longitude_degs -
                                   previous_report.longitude_degs) /
                                  interval_secs * age_secs;
    }
  }

  const double reference_latitude_rads =
      (report.latitude_degs + predicted_latitude_degs) * 0.5 *
      kDegreesToRadians;
  const double north_m =
      (report.latitude_degs - predicted_latitude_degs) * kMetersPerDegree;
  const double east_m = (report.longitude_degs - predicted_longitude_degs) *
                        kMetersPerDegree * std::cos(reference_latitude_rads);
  return std::hypot(north_m, east_m);
}

}  // namespace correlator
