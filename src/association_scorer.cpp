#include "association_scorer.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "geo_utils.h"
#include "identifier_matcher.h"

namespace correlator {

AssociationScorer::AssociationScorer(AssociationConfig config)
    : config_(config) {
  if (!std::isfinite(config_.max_track_age_secs) ||
      config_.max_track_age_secs <= 0.0 ||
      !std::isfinite(config_.max_distance_m) || config_.max_distance_m <= 0.0 ||
      !std::isfinite(config_.min_score) ||
      !std::isfinite(config_.min_score_margin) ||
      config_.min_score_margin < 0.0 ||
      !std::isfinite(config_.distance_weight) ||
      !std::isfinite(config_.age_weight) ||
      !std::isfinite(config_.identifier_weight) ||
      !std::isfinite(config_.altitude_weight) ||
      config_.distance_weight < 0.0 || config_.age_weight < 0.0 ||
      config_.identifier_weight < 0.0 || config_.altitude_weight < 0.0) {
    throw std::invalid_argument("Association configuration is invalid");
  }
}

AssociationResult AssociationScorer::Evaluate(
    const Report& report, std::span<const TrackState> tracks) const {
  std::vector<const TrackState*> track_references;
  track_references.reserve(tracks.size());
  for (const TrackState& track : tracks) {
    track_references.push_back(&track);
  }
  return Evaluate(report, track_references);
}

AssociationResult AssociationScorer::Evaluate(
    const Report& report, std::span<const TrackState* const> tracks) const {
  AssociationResult result;
  for (const TrackState* track : tracks) {
    const double age_secs =
        report.arrival_time_secs - track->latest_report.arrival_time_secs;
    if (age_secs < 0.0 || age_secs > config_.max_track_age_secs) {
      continue;
    }

    const IdentifierRelation identifier_relation =
        IdentifierMatcher::Compare(report, *track);
    if (report.identifier.has_value() &&
        identifier_relation == IdentifierRelation::kConflict) {
      result.rejected_for_identifier_conflict = true;
      continue;
    }
    if (report.altitude_m.has_value() !=
        track->latest_report.altitude_m.has_value()) {
      continue;
    }
    const double distance_m = CalculateDistanceM(report, *track, age_secs);
    if (distance_m > config_.max_distance_m) {
      continue;
    }

    const double distance_score = 1.0 - distance_m / config_.max_distance_m;
    const double age_score = 1.0 - age_secs / config_.max_track_age_secs;
    const double identifier_score =
        identifier_relation == IdentifierRelation::kMatch      ? 1.0
        : identifier_relation == IdentifierRelation::kConflict ? 0.0
                                                               : 0.5;
    double altitude_score = 0.5;
    if (report.altitude_m.has_value() &&
        track->latest_report.altitude_m.has_value()) {
      const double altitude_difference_m = std::abs(
          report.altitude_m.value() - track->latest_report.altitude_m.value());
      altitude_score =
          std::max(0.0, 1.0 - altitude_difference_m / kAltitudeEvidenceRangeM);
    }
    const double score = config_.distance_weight * distance_score +
                         config_.age_weight * age_score +
                         config_.identifier_weight * identifier_score +
                         config_.altitude_weight * altitude_score;
    result.candidates.push_back(AssociationCandidate{
        .track_id = track->track_id, .score = score, .distance_m = distance_m});
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
  const Report* latest_observation = nullptr;
  for (const Report& observation : track.history) {
    if (observation.receiver_time_secs <= report.receiver_time_secs &&
        (latest_observation == nullptr ||
         observation.receiver_time_secs >
             latest_observation->receiver_time_secs)) {
      latest_observation = &observation;
    }
  }
  if (latest_observation == nullptr) {
    latest_observation = &track.latest_report;
  }

  double predicted_latitude_degs = latest_observation->latitude_degs;
  double predicted_longitude_degs = latest_observation->longitude_degs;
  const Report* previous_observation = nullptr;
  for (const Report& observation : track.history) {
    const double interval_secs =
        latest_observation->receiver_time_secs - observation.receiver_time_secs;
    if (interval_secs >= kMinimumVelocityIntervalSecs &&
        (previous_observation == nullptr ||
         observation.receiver_time_secs >
             previous_observation->receiver_time_secs)) {
      previous_observation = &observation;
    }
  }
  if (previous_observation != nullptr) {
    const double interval_secs = latest_observation->receiver_time_secs -
                                 previous_observation->receiver_time_secs;
    const double prediction_age_secs =
        report.receiver_time_secs - latest_observation->receiver_time_secs;
    const double distance_m = CalculateHorizontalDistanceM(
        latest_observation->latitude_degs, latest_observation->longitude_degs,
        previous_observation->latitude_degs,
        previous_observation->longitude_degs);
    if (prediction_age_secs > 0.0 && distance_m > 0.0) {
      const double scale =
          std::min(distance_m / interval_secs * prediction_age_secs,
                   kMaximumPredictionSpeedMps * prediction_age_secs) /
          distance_m;
      predicted_latitude_degs += (latest_observation->latitude_degs -
                                  previous_observation->latitude_degs) *
                                 scale;
      predicted_longitude_degs += (latest_observation->longitude_degs -
                                   previous_observation->longitude_degs) *
                                  scale;
    }
  }

  return CalculateHorizontalDistanceM(
      report.latitude_degs, report.longitude_degs, predicted_latitude_degs,
      predicted_longitude_degs);
}

}  // namespace correlator
