#include "answer_writer.h"

#include <nlohmann/json.hpp>
#include <ostream>

namespace correlator {

void AnswerWriter::Write(std::ostream& output, const Answer& answer) {
  const nlohmann::json serialized_answer{
      {"report", answer.report_index},
      {"track", answer.track_id.has_value()
                    ? nlohmann::json(answer.track_id.value())
                    : nlohmann::json(nullptr)},
      {"at", answer.at_secs},
  };
  output << serialized_answer << '\n';
}

}  // namespace correlator
