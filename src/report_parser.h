#ifndef INVOLI_TRACK_CORRELATOR_SRC_REPORT_PARSER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_REPORT_PARSER_H_

#include <cstddef>
#include <string_view>

#include "report.h"

namespace correlator {

// Parses one JSONL input line into the typed report representation.
class ReportParser {
 public:
  // Returns a report or a structured parse error without writing to stdout.
  [[nodiscard]] static ParseResult Parse(std::string_view line,
                                         std::size_t report_index);
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_REPORT_PARSER_H_
