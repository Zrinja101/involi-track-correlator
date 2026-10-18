#include "report_validator.h"

#include <cmath>
#include <string>

namespace correlator {
namespace {

constexpr double kMinLatitudeDegs = -90.0;
constexpr double kMaxLatitudeDegs = 90.0;
constexpr double kMinLongitudeDegs = -180.0;
constexpr double kMaxLongitudeDegs = 180.0;

ValidationResult Failure(ValidationErrorCode code, const char* message) {
  return ValidationResult{
      .error = ValidationError{.code = code, .message = std::string(message)}};
}

bool IsFinite(const Report& report) {
  return std::isfinite(report.arrival_time_secs) &&
         std::isfinite(report.receiver_time_secs) &&
         std::isfinite(report.latitude_degs) &&
         std::isfinite(report.longitude_degs) &&
         (!report.altitude_m.has_value() ||
          std::isfinite(report.altitude_m.value()));
}

}  // namespace

ValidationResult ReportValidator::Validate(const Report& report) {
  if (!IsFinite(report)) {
    return Failure(ValidationErrorCode::kNonFiniteValue,
                   "Report contains a non-finite numeric value");
  }
  if (report.receiver.empty()) {
    return Failure(ValidationErrorCode::kEmptyReceiver,
                   "Receiver name must not be empty");
  }
  if (report.latitude_degs < kMinLatitudeDegs ||
      report.latitude_degs > kMaxLatitudeDegs) {
    return Failure(ValidationErrorCode::kLatitudeOutOfRange,
                   "Latitude is outside [-90, 90] degrees");
  }
  if (report.longitude_degs < kMinLongitudeDegs ||
      report.longitude_degs > kMaxLongitudeDegs) {
    return Failure(ValidationErrorCode::kLongitudeOutOfRange,
                   "Longitude is outside [-180, 180] degrees");
  }
  return ValidationResult{.error = std::nullopt};
}

}  // namespace correlator
