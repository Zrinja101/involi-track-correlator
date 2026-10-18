#ifndef INVOLI_TRACK_CORRELATOR_SRC_ANSWER_WRITER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_ANSWER_WRITER_H_

#include <cstddef>
#include <iosfwd>
#include <optional>
#include <string>

namespace correlator {

struct Answer {
  std::size_t report_index;
  std::optional<std::string> track_id;
  double at_secs;
};

// Serializes one answer as a machine-readable JSONL record.
class AnswerWriter {
 public:
  static void Write(std::ostream& output, const Answer& answer);
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_ANSWER_WRITER_H_
