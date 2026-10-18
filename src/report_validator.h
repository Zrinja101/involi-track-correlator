#ifndef INVOLI_TRACK_CORRELATOR_SRC_REPORT_VALIDATOR_H_
#define INVOLI_TRACK_CORRELATOR_SRC_REPORT_VALIDATOR_H_

#include <optional>
#include <string>

#include "report.h"

namespace correlator {

enum class ValidationErrorCode {
  kEmptyReceiver,
  kLatitudeOutOfRange,
  kLongitudeOutOfRange,
  kNonFiniteValue,
};

struct ValidationError {
  ValidationErrorCode code;
  std::string message;
};

struct ValidationResult {
  std::optional<ValidationError> error;

  [[nodiscard]] bool IsValid() const noexcept { return !error.has_value(); }
};

// Validates physical and operational invariants of a parsed report.
class ReportValidator {
 public:
  // Returns a validation error without modifying the report or writing output.
  [[nodiscard]] static ValidationResult Validate(const Report& report);
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_REPORT_VALIDATOR_H_
