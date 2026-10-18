#ifndef INVOLI_TRACK_CORRELATOR_SRC_REPORT_VALIDATOR_H_
#define INVOLI_TRACK_CORRELATOR_SRC_REPORT_VALIDATOR_H_

#include <optional>
#include <string>

#include "report.h"

namespace correlator {

/// Classification of report-domain validation failures.
enum class ValidationErrorCode {
  kEmptyReceiver,
  kLatitudeOutOfRange,
  kLongitudeOutOfRange,
  kNonFiniteValue,
};

/// Details for a report that violates a domain invariant.
struct ValidationError {
  /// Machine-readable failure category.
  ValidationErrorCode code;
  /// Human-readable diagnostic.
  std::string message;
};

/// Result of validating a parsed report.
struct ValidationResult {
  /// Present when a report-domain invariant is violated.
  std::optional<ValidationError> error;

  /// Returns true when all report-domain invariants hold.
  [[nodiscard]] bool IsValid() const noexcept { return !error.has_value(); }
};

// Validates physical and operational invariants of a parsed report.
class ReportValidator {
 public:
  /// Returns a validation error without modifying the report or writing output.
  ///
  /// Checks finite numeric values, a nonempty receiver, and WGS84 coordinate
  /// bounds. Temporal ordering and motion are checked by other components.
  [[nodiscard]] static ValidationResult Validate(const Report& report);
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_REPORT_VALIDATOR_H_
