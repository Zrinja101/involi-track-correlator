#ifndef INVOLI_TRACK_CORRELATOR_SRC_ANSWER_WRITER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_ANSWER_WRITER_H_

#include <cstddef>
#include <iosfwd>
#include <optional>
#include <string>

namespace correlator {

/// One externally visible correlator decision.
struct Answer {
  /// Zero-based input report index answered by this record.
  std::size_t report_index;
  /// Assigned track name, or nullopt when the report is rejected.
  std::optional<std::string> track_id;
  /// Decision time on the arrival clock, in seconds.
  double at_secs;
};

// Serializes one answer as a machine-readable JSONL record.
class AnswerWriter {
 public:
  /// Writes one JSON object followed by a newline.
  ///
  /// The output contains exactly `report`, `track`, and `at` fields and never
  /// writes diagnostics to the answer stream.
  static void Write(std::ostream& output, const Answer& answer);
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_ANSWER_WRITER_H_
