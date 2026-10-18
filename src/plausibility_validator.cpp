#include "plausibility_validator.h"

#include <cmath>
#include <numbers>
#include <string>

namespace correlator {
namespace {

constexpr double kMetersPerDegree = 111320.0;
constexpr double kDegreesToRadians = std::numbers::pi / 180.0;

PlausibilityResult Failure(PlausibilityErrorCode code, const char* message) {
  return PlausibilityResult{.error = PlausibilityError{
                                .code = code, .message = std::string(message)}};
}

double HorizontalDistanceM(const Report& report,
                           const Report& previous_report) {
  const double reference_latitude_rads =
      (report.latitude_degs + previous_report.latitude_degs) * 0.5 *
      kDegreesToRadians;
  const double north_m =
      (report.latitude_degs - previous_report.latitude_degs) * kMetersPerDegree;
  const double east_m =
      (report.longitude_degs - previous_report.longitude_degs) *
      kMetersPerDegree * std::cos(reference_latitude_rads);
  return std::hypot(north_m, east_m);
}

}  // namespace

PlausibilityValidator::PlausibilityValidator(PlausibilityConfig config)
    : config_(config) {}

PlausibilityResult PlausibilityValidator::Validate(
    const Report& report, const Report& previous_report) const {
  const double elapsed_secs =
      report.arrival_time_secs - previous_report.arrival_time_secs;
  if (elapsed_secs < 0.0) {
    return Failure(PlausibilityErrorCode::kNonIncreasingTime,
                   "Report arrival time precedes the prior observation");
  }
  if (elapsed_secs == 0.0) {
    if (HorizontalDistanceM(report, previous_report) == 0.0 &&
        report.altitude_m == previous_report.altitude_m) {
      return PlausibilityResult{.error = std::nullopt};
    }
    return Failure(PlausibilityErrorCode::kNonIncreasingTime,
                   "Different observations share an arrival timestamp");
  }

  if (HorizontalDistanceM(report, previous_report) / elapsed_secs >
      config_.max_horizontal_speed_mps) {
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
