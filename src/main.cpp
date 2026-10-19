#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "answer_writer.h"
#include "arrival_scheduler.h"
#include "association_scorer.h"
#include "report_parser.h"
#include "report_validator.h"
#include "track_store.h"

int main() try {
  correlator::ArrivalScheduler scheduler;
  correlator::TrackStore track_store;
  correlator::AssociationScorer association_scorer;
  std::unordered_map<std::size_t, correlator::Report> pending_reports;
  std::size_t report_index = 0U;
  double last_at_secs = 0.0;
  bool has_last_at = false;

  const auto write_decisions = [&](const std::vector<correlator::Decision>&
                                       decisions) {
    for (const correlator::Decision& decision : decisions) {
      const auto pending_iterator = pending_reports.find(decision.report_index);
      if (pending_iterator == pending_reports.end()) {
        std::cerr << "report " << decision.report_index
                  << ": pending report unavailable\n";
        continue;
      }

      correlator::Report report = std::move(pending_iterator->second);
      pending_reports.erase(pending_iterator);
      static_cast<void>(
          track_store.RetireStaleTracks(report.arrival_time_secs));
      const std::vector<const correlator::TrackState*> tracks =
          track_store.Snapshot();
      const correlator::AssociationResult association =
          association_scorer.Evaluate(report, tracks);
      std::optional<std::string> track_id;

      if (association.assigned_track_id.has_value()) {
        const correlator::TrackState* track =
            track_store.FindTrack(association.assigned_track_id.value());
        if (track != nullptr &&
            track_store.UpdateTrack(association.assigned_track_id.value(),
                                    report)) {
          track_id = association.assigned_track_id;
        } else {
          std::cerr << "report " << report.report_index
                    << ": candidate track update failed\n";
        }
      } else if (association.candidates.empty()) {
        track_id = track_store.CreateTrack(report);
        if (!track_id.has_value()) {
          std::cerr << "report " << report.report_index
                    << ": active-track capacity reached\n";
        }
      } else if (association.rejected_for_identifier_conflict) {
        std::cerr << "report " << report.report_index
                  << ": rejected for contradictory identifier and position\n";
      } else {
        std::cerr << "report " << report.report_index
                  << ": rejected as ambiguous\n";
      }

      correlator::AnswerWriter::Write(
          std::cout, correlator::Answer{.report_index = decision.report_index,
                                        .track_id = track_id,
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

    pending_reports.emplace(report.report_index, report);
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
