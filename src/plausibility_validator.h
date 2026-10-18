#ifndef INVOLI_TRACK_CORRELATOR_SRC_PLAUSIBILITY_VALIDATOR_H_
#define INVOLI_TRACK_CORRELATOR_SRC_PLAUSIBILITY_VALIDATOR_H_

#include <optional>
#include <string>

#include "report.h"

namespace correlator {

/// Classification of contextual motion failures.
enum class PlausibilityErrorCode {
  kNonIncreasingTime,
  kHorizontalSpeedExceeded,
  kVerticalSpeedExceeded,
};

/// Details for a report that violates the motion model.
struct PlausibilityError {
  /// Machine-readable failure category.
  PlausibilityErrorCode code;
  /// Human-readable diagnostic.
  std::string message;
};

/// Result of contextual plausibility validation.
struct PlausibilityResult {
  /// Present when the report violates the configured motion model.
  std::optional<PlausibilityError> error;

  /// Returns true when the report is plausible relative to its predecessor.
  [[nodiscard]] bool IsValid() const noexcept { return !error.has_value(); }
};

/// Physical movement limits used by PlausibilityValidator.
struct PlausibilityConfig {
  /// Maximum horizontal speed, in metres per second.
  double max_horizontal_speed_mps = 500.0;
  /// Maximum vertical speed, in metres per second.
  double max_vertical_speed_mps = 150.0;
};

// Checks motion between a new report and a prior accepted observation.
class PlausibilityValidator {
 public:
  /// Constructs a validator with explicit physical speed limits.
  explicit PlausibilityValidator(PlausibilityConfig config = {});

  /// Compares a report with the preceding accepted observation.
  ///
  /// Equal timestamps are accepted only when position and altitude are equal.
  [[nodiscard]] PlausibilityResult Validate(
      const Report& report, const Report& previous_report) const;

 private:
  PlausibilityConfig config_;
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_PLAUSIBILITY_VALIDATOR_H_
