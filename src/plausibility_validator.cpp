#include "plausibility_validator.h"

#include <cmath>
#include <stdexcept>
#include <string>

#include "geo_utils.h"

namespace correlator {
namespace {

PlausibilityResult Failure(PlausibilityErrorCode code, const char* message) {
  return PlausibilityResult{.error = PlausibilityError{
                                .code = code, .message = std::string(message)}};
}

}  // namespace

PlausibilityValidator::PlausibilityValidator(PlausibilityConfig config)
    : config_(config) {
  if (!std::isfinite(config_.max_horizontal_speed_mps) ||
      config_.max_horizontal_speed_mps <= 0.0 ||
      !std::isfinite(config_.max_vertical_speed_mps) ||
      config_.max_vertical_speed_mps <= 0.0) {
    throw std::invalid_argument("Plausibility configuration is invalid");
  }
}

PlausibilityResult PlausibilityValidator::Validate(
    const Report& report, const Report& previous_report) const {
  const double elapsed_secs =
      report.receiver_time_secs - previous_report.receiver_time_secs;
  if (elapsed_secs < 0.0) {
    return Failure(PlausibilityErrorCode::kNonIncreasingTime,
                   "Report arrival time precedes the prior observation");
  }
  if (elapsed_secs == 0.0) {
    if (CalculateHorizontalDistanceM(report.latitude_degs,
                                     report.longitude_degs,
                                     previous_report.latitude_degs,
                                     previous_report.longitude_degs) == 0.0 &&
        report.altitude_m == previous_report.altitude_m) {
      return PlausibilityResult{.error = std::nullopt};
    }
    return Failure(PlausibilityErrorCode::kNonIncreasingTime,
                   "Different observations share an arrival timestamp");
  }

  const double horizontal_distance_m = CalculateHorizontalDistanceM(
      report.latitude_degs, report.longitude_degs,
      previous_report.latitude_degs, previous_report.longitude_degs);
  if (horizontal_distance_m / elapsed_secs > config_.max_horizontal_speed_mps) {
    return Failure(PlausibilityErrorCode::kHorizontalSpeedExceeded,
                   "Horizontal movement exceeds the configured speed limit");
  }

  if (report.altitude_m.has_value() && previous_report.altitude_m.has_value() &&
      std::abs(report.altitude_m.value() - previous_report.altitude_m.value()) /
              elapsed_secs >
          config_.max_vertical_speed_mps) {
    return Failure(PlausibilityErrorCode::kVerticalSpeedExceeded,
                   "Vertical movement exceeds the configured speed limit");
  }
  return PlausibilityResult{.error = std::nullopt};
}

}  // namespace correlator
