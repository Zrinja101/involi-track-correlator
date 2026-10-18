#ifndef INVOLI_TRACK_CORRELATOR_SRC_REPORT_H_
#define INVOLI_TRACK_CORRELATOR_SRC_REPORT_H_

#include <cstddef>
#include <optional>
#include <string>

namespace correlator {

// A syntactically valid observation received by the correlator.
struct Report {
  std::size_t report_index;
  double arrival_time_secs;
  std::string receiver;
  double receiver_time_secs;
  std::optional<std::string> identifier;
  double latitude_degs;
  double longitude_degs;
  std::optional<double> altitude_m;
};

enum class ParseErrorCode {
  kMalformedJson,
  kMissingField,
  kInvalidType,
  kInvalidValue,
};

struct ParseError {
  ParseErrorCode code;
  std::string message;
};

struct ParseResult {
  std::optional<Report> report;
  std::optional<ParseError> error;

  [[nodiscard]] bool IsSuccess() const noexcept {
    return report.has_value() && !error.has_value();
  }
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_REPORT_H_
