#ifndef INVOLI_TRACK_CORRELATOR_SRC_RUNTIME_CONFIG_H_
#define INVOLI_TRACK_CORRELATOR_SRC_RUNTIME_CONFIG_H_

#include <cstddef>
#include <string>

#include "arrival_scheduler.h"
#include "association_scorer.h"
#include "plausibility_validator.h"
#include "track_store.h"

namespace correlator {

/// Complete immutable startup configuration for the correlator.
struct RuntimeConfig {
  AssociationConfig association;
  PlausibilityConfig plausibility;
  std::size_t max_pending_reports = kDefaultMaxPendingReports;
  double decision_window_secs = kDecisionWindowSecs;
  std::size_t max_active_tracks = kDefaultMaxActiveTracks;
  std::size_t max_track_history = kDefaultMaxTrackHistory;
  double track_retirement_secs = kDefaultTrackRetirementSecs;
};

/// Returns the built-in configuration after validating all limits.
[[nodiscard]] RuntimeConfig DefaultRuntimeConfig();

/// Loads and validates a JSON configuration file.
///
/// Throws std::runtime_error with a path-oriented diagnostic when the file is
/// unavailable, malformed, contains an unknown field, or has an invalid value.
[[nodiscard]] RuntimeConfig LoadRuntimeConfig(const std::string& path);

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_RUNTIME_CONFIG_H_