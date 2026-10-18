#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "answer_writer.h"
#include "arrival_scheduler.h"
#include "report_parser.h"
#include "report_validator.h"

int main() try {
  correlator::ArrivalScheduler scheduler;
  std::size_t report_index = 0U;
  double last_at_secs = 0.0;
  bool has_last_at = false;

  const auto write_decisions =
      [&](const std::vector<correlator::Decision>& decisions) {
        for (const correlator::Decision& decision : decisions) {
          correlator::AnswerWriter::Write(
              std::cout,
              correlator::Answer{.report_index = decision.report_index,
                                 .track_id = std::nullopt,
                                 .at_secs = decision.at_secs});
          last_at_secs = decision.at_secs;
          has_last_at = true;
        }
      };

  std::string line;
  while (std::getline(std::cin, line)) {
    const correlator::ParseResult parsed_report =
        correlator::ReportParser::Parse(line, report_index);
    if (!parsed_report.IsSuccess()) {
      const std::string error_message = parsed_report.error.has_value()
                                            ? parsed_report.error->message
                                            : "Report parsing failed";
      std::cerr << "report " << report_index << ": " << error_message << '\n';
      correlator::AnswerWriter::Write(
          std::cout,
          correlator::Answer{.report_index = report_index,
                             .track_id = std::nullopt,
                             .at_secs = has_last_at ? last_at_secs : 0.0});
      ++report_index;
      continue;
    }

    if (!parsed_report.report.has_value()) {
      std::cerr << "report " << report_index << ": Report is unavailable\n";
      correlator::AnswerWriter::Write(
          std::cout,
          correlator::Answer{.report_index = report_index,
                             .track_id = std::nullopt,
                             .at_secs = has_last_at ? last_at_secs : 0.0});
      ++report_index;
      continue;
    }
    const correlator::Report& report = *parsed_report.report;
    const correlator::ValidationResult validation =
        correlator::ReportValidator::Validate(report);
    if (!validation.IsValid()) {
      const std::string error_message = validation.error.has_value()
                                            ? validation.error->message
                                            : "Report validation failed";
      std::cerr << "report " << report_index << ": " << error_message << '\n';
      write_decisions(scheduler.AdvanceTo(report.arrival_time_secs));
      const double at_secs =
          has_last_at ? std::max(last_at_secs, report.arrival_time_secs)
                      : report.arrival_time_secs;
      correlator::AnswerWriter::Write(
          std::cout, correlator::Answer{.report_index = report_index,
                                        .track_id = std::nullopt,
                                        .at_secs = at_secs});
      last_at_secs = at_secs;
      has_last_at = true;
      ++report_index;
      continue;
    }

    write_decisions(
        scheduler.Submit(report.report_index, report.arrival_time_secs));
    ++report_index;
  }

  write_decisions(scheduler.Flush());
  return 0;
} catch (const std::exception& error) {
  std::cerr << "fatal: " << error.what() << '\n';
  return 1;
}
