#ifndef INVOLI_TRACK_CORRELATOR_SRC_REPORT_H_
#define INVOLI_TRACK_CORRELATOR_SRC_REPORT_H_

#include <cstddef>
#include <optional>
#include <string>

namespace correlator {

/// A syntactically valid observation received by the correlator.
///
/// Time fields are expressed in seconds and geographic fields in WGS84
/// degrees. Optional fields are absent when the input did not provide them.
struct Report {
  /// Zero-based input line number assigned by the correlator.
  std::size_t report_index;
  /// Arrival time on the correlator clock, in seconds.
  double arrival_time_secs;
  /// Name of the receiver that produced the observation.
  std::string receiver;
  /// Observation time on the receiver clock, in seconds.
  double receiver_time_secs;
  /// Optional aircraft-broadcast identifier.
  std::optional<std::string> identifier;
  /// WGS84 latitude in degrees.
  double latitude_degs;
  /// WGS84 longitude in degrees.
  double longitude_degs;
  /// Optional altitude in metres.
  std::optional<double> altitude_m;
};

/// Classification of failures encountered while decoding a JSONL report.
enum class ParseErrorCode {
  kMalformedJson,
  kMissingField,
  kInvalidType,
  kInvalidValue,
};

/// Details for a report that could not be decoded.
struct ParseError {
  /// Machine-readable failure category.
  ParseErrorCode code;
  /// Human-readable diagnostic suitable for stderr or logs.
  std::string message;
};

/// Result of parsing one input line.
struct ParseResult {
  /// Present only when decoding and basic numeric checks succeed.
  std::optional<Report> report;
  /// Present when parsing fails.
  std::optional<ParseError> error;

  /// Returns true when a report is available and no parse error exists.
  [[nodiscard]] bool IsSuccess() const noexcept {
    return report.has_value() && !error.has_value();
  }
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_REPORT_H_
