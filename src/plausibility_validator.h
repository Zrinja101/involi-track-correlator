#ifndef INVOLI_TRACK_CORRELATOR_SRC_PLAUSIBILITY_VALIDATOR_H_
#define INVOLI_TRACK_CORRELATOR_SRC_PLAUSIBILITY_VALIDATOR_H_

#include <optional>
#include <string>

#include "report.h"

namespace correlator {

enum class PlausibilityErrorCode {
  kNonIncreasingTime,
  kHorizontalSpeedExceeded,
  kVerticalSpeedExceeded,
};

struct PlausibilityError {
  PlausibilityErrorCode code;
  std::string message;
};

struct PlausibilityResult {
  std::optional<PlausibilityError> error;

  [[nodiscard]] bool IsValid() const noexcept { return !error.has_value(); }
};

struct PlausibilityConfig {
  double max_horizontal_speed_mps = 500.0;
  double max_vertical_speed_mps = 150.0;
};

// Checks motion between a new report and a prior accepted observation.
class PlausibilityValidator {
 public:
  explicit PlausibilityValidator(PlausibilityConfig config = {});

  [[nodiscard]] PlausibilityResult Validate(
      const Report& report, const Report& previous_report) const;

 private:
  PlausibilityConfig config_;
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_PLAUSIBILITY_VALIDATOR_H_
