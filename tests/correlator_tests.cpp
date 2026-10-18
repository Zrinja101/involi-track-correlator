#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "answer_writer.h"
#include "arrival_scheduler.h"
#include "association_scorer.h"
#include "identifier_matcher.h"
#include "plausibility_validator.h"
#include "report_parser.h"
#include "report_validator.h"
#include "track_store.h"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// readability-identifier-length, bugprone-easily-swappable-parameters)

TEST_CASE("ReportParser parses a complete report") {
  const std::string line =
      R"({"arr":36004.518,"rx":"R3","t":36003.263,"id":"AFAA7C","lat":47.065481,"lon":7.428459,"alt":784.9})";

  const correlator::ParseResult result =
      correlator::ReportParser::Parse(line, 17U);

  REQUIRE(result.IsSuccess());
  REQUIRE(result.report.has_value());
  CHECK(result.report->report_index == 17U);
  CHECK(result.report->arrival_time_secs == 36004.518);
  CHECK(result.report->receiver == "R3");
  CHECK(result.report->receiver_time_secs == 36003.263);
  REQUIRE(result.report->identifier.has_value());
  CHECK(result.report->identifier.value() == "AFAA7C");
  CHECK(result.report->latitude_degs == 47.065481);
  CHECK(result.report->longitude_degs == 7.428459);
  REQUIRE(result.report->altitude_m.has_value());
  CHECK(result.report->altitude_m.value() == 784.9);
}

TEST_CASE("ReportParser preserves absent optional fields") {
  const std::string line =
      R"({"arr":10.0,"rx":"R1","t":9.5,"lat":47.0,"lon":7.0})";

  const correlator::ParseResult result =
      correlator::ReportParser::Parse(line, 2U);

  REQUIRE(result.IsSuccess());
  REQUIRE(result.report.has_value());
  CHECK_FALSE(result.report->identifier.has_value());
  CHECK_FALSE(result.report->altitude_m.has_value());
}

TEST_CASE("ReportParser accepts whitespace, integer values, and extra fields") {
  const std::string line =
      R"( {"arr":10,"rx":"","t":9,"id":"","lat":47,"lon":7,"extra":true} )";

  const correlator::ParseResult result =
      correlator::ReportParser::Parse(line, 3U);

  REQUIRE(result.IsSuccess());
  REQUIRE(result.report.has_value());
  CHECK(result.report->receiver.empty());
  REQUIRE(result.report->identifier.has_value());
  CHECK(result.report->identifier->empty());
  CHECK(result.report->arrival_time_secs == 10.0);
}

TEST_CASE("ReportParser rejects malformed JSON") {
  const correlator::ParseResult result =
      correlator::ReportParser::Parse("{", 0U);

  REQUIRE_FALSE(result.IsSuccess());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ParseErrorCode::kMalformedJson);
}

TEST_CASE("ReportParser rejects missing required fields") {
  const std::string line = R"({"arr":10.0,"rx":"R1","t":9.5,"lat":47.0})";

  const correlator::ParseResult result =
      correlator::ReportParser::Parse(line, 0U);

  REQUIRE_FALSE(result.IsSuccess());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ParseErrorCode::kMissingField);
}

TEST_CASE("ReportParser rejects fields with incorrect types") {
  const std::string line =
      R"({"arr":"10.0","rx":"R1","t":9.5,"lat":47.0,"lon":7.0})";

  const correlator::ParseResult result =
      correlator::ReportParser::Parse(line, 0U);

  REQUIRE_FALSE(result.IsSuccess());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ParseErrorCode::kInvalidType);
}

TEST_CASE("ReportParser rejects a non-object JSON value") {
  const correlator::ParseResult result =
      correlator::ReportParser::Parse("[]", 0U);

  REQUIRE_FALSE(result.IsSuccess());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ParseErrorCode::kInvalidType);
}

TEST_CASE("ReportParser rejects an optional field with an incorrect type") {
  const std::string line =
      R"({"arr":10.0,"rx":"R1","t":9.5,"lat":47.0,"lon":7.0,"alt":"unknown"})";

  const correlator::ParseResult result =
      correlator::ReportParser::Parse(line, 0U);

  REQUIRE_FALSE(result.IsSuccess());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ParseErrorCode::kInvalidType);
}

namespace {

correlator::Report MakeValidReport() {
  return correlator::Report{
      .report_index = 0U,
      .arrival_time_secs = 10.0,
      .receiver = "R1",
      .receiver_time_secs = 9.5,
      .identifier = std::nullopt,
      .latitude_degs = 47.0,
      .longitude_degs = 7.0,
      .altitude_m = 500.0,
  };
}

correlator::TrackState MakeTrack(std::string track_id,
                                 const correlator::Report& report) {
  return correlator::TrackState{
      .track_id = std::move(track_id),
      .latest_report = report,
      .history = {report},
  };
}

}  // namespace

TEST_CASE("ReportValidator accepts geographic domain boundaries") {
  correlator::Report report = MakeValidReport();

  report.latitude_degs = -90.0;
  report.longitude_degs = -180.0;
  CHECK(correlator::ReportValidator::Validate(report).IsValid());

  report.latitude_degs = 90.0;
  report.longitude_degs = 180.0;
  CHECK(correlator::ReportValidator::Validate(report).IsValid());
}

TEST_CASE("ReportValidator rejects latitude outside its domain") {
  correlator::Report report = MakeValidReport();
  report.latitude_degs = 90.000001;

  const correlator::ValidationResult result =
      correlator::ReportValidator::Validate(report);

  REQUIRE_FALSE(result.IsValid());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code ==
        correlator::ValidationErrorCode::kLatitudeOutOfRange);
}

TEST_CASE("ReportValidator rejects longitude outside its domain") {
  correlator::Report report = MakeValidReport();
  report.longitude_degs = -180.000001;

  const correlator::ValidationResult result =
      correlator::ReportValidator::Validate(report);

  REQUIRE_FALSE(result.IsValid());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code ==
        correlator::ValidationErrorCode::kLongitudeOutOfRange);
}

TEST_CASE("ReportValidator rejects an empty receiver") {
  correlator::Report report = MakeValidReport();
  report.receiver.clear();

  const correlator::ValidationResult result =
      correlator::ReportValidator::Validate(report);

  REQUIRE_FALSE(result.IsValid());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ValidationErrorCode::kEmptyReceiver);
}

TEST_CASE("ReportValidator rejects non-finite report values") {
  correlator::Report report = MakeValidReport();
  report.arrival_time_secs = std::numeric_limits<double>::infinity();

  const correlator::ValidationResult result =
      correlator::ReportValidator::Validate(report);

  REQUIRE_FALSE(result.IsValid());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ValidationErrorCode::kNonFiniteValue);
}

TEST_CASE("ReportValidator rejects a non-finite altitude") {
  correlator::Report report = MakeValidReport();
  report.altitude_m = std::numeric_limits<double>::quiet_NaN();

  const correlator::ValidationResult result =
      correlator::ReportValidator::Validate(report);

  REQUIRE_FALSE(result.IsValid());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == correlator::ValidationErrorCode::kNonFiniteValue);
}

TEST_CASE("PlausibilityValidator accepts realistic movement") {
  const correlator::Report previous_report = MakeValidReport();
  correlator::Report report = previous_report;
  report.arrival_time_secs = 11.0;
  report.longitude_degs += 0.001;

  CHECK(correlator::PlausibilityValidator()
            .Validate(report, previous_report)
            .IsValid());
}

TEST_CASE("PlausibilityValidator rejects an implausible horizontal jump") {
  const correlator::Report previous_report = MakeValidReport();
  correlator::Report report = previous_report;
  report.arrival_time_secs = 11.0;
  report.latitude_degs += 0.1;

  const correlator::PlausibilityResult result =
      correlator::PlausibilityValidator().Validate(report, previous_report);

  REQUIRE_FALSE(result.IsValid());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code ==
        correlator::PlausibilityErrorCode::kHorizontalSpeedExceeded);
}

TEST_CASE("PlausibilityValidator rejects an implausible altitude jump") {
  const correlator::Report previous_report = MakeValidReport();
  correlator::Report report = previous_report;
  report.arrival_time_secs = 11.0;
  report.altitude_m = 1000.0;

  const correlator::PlausibilityResult result =
      correlator::PlausibilityValidator().Validate(report, previous_report);

  REQUIRE_FALSE(result.IsValid());
  REQUIRE(result.error.has_value());
  CHECK(result.error->code ==
        correlator::PlausibilityErrorCode::kVerticalSpeedExceeded);
}

TEST_CASE("PlausibilityValidator accepts duplicate-time stationary reports") {
  const correlator::Report previous_report = MakeValidReport();
  const correlator::PlausibilityResult result =
      correlator::PlausibilityValidator().Validate(previous_report,
                                                   previous_report);

  CHECK(result.IsValid());
}

TEST_CASE("ArrivalScheduler waits until a report deadline") {
  correlator::ArrivalScheduler scheduler;

  CHECK(scheduler.Submit(4U, 10.0).empty());
  CHECK(scheduler.AdvanceTo(11.999).empty());

  const std::vector<correlator::Decision> decisions = scheduler.AdvanceTo(12.0);

  REQUIRE(decisions.size() == 1U);
  CHECK(decisions.front().report_index == 4U);
  CHECK(decisions.front().at_secs == 12.0);
}

TEST_CASE("ArrivalScheduler orders equal deadlines deterministically") {
  correlator::ArrivalScheduler scheduler;

  CHECK(scheduler.Submit(9U, 10.0).empty());
  CHECK(scheduler.Submit(3U, 10.0).empty());

  const std::vector<correlator::Decision> decisions = scheduler.AdvanceTo(12.0);

  REQUIRE(decisions.size() == 2U);
  CHECK(decisions[0].report_index == 3U);
  CHECK(decisions[1].report_index == 9U);
  CHECK(decisions[0].at_secs <= decisions[1].at_secs);
}

TEST_CASE("ArrivalScheduler finalizes due reports before a large gap") {
  correlator::ArrivalScheduler scheduler;

  const std::vector<correlator::Decision> decisions =
      scheduler.Submit(8U, 20.0);

  CHECK(decisions.empty());
  const std::vector<correlator::Decision> later_decisions =
      scheduler.Submit(2U, 30.0);

  REQUIRE(later_decisions.size() == 1U);
  CHECK(later_decisions.front().report_index == 8U);
  CHECK(later_decisions.front().at_secs == 22.0);
}

TEST_CASE("ArrivalScheduler flushes pending reports within their window") {
  correlator::ArrivalScheduler scheduler;

  CHECK(scheduler.Submit(6U, 5.0).empty());
  const std::vector<correlator::Decision> decisions = scheduler.Flush();

  REQUIRE(decisions.size() == 1U);
  CHECK(decisions.front().report_index == 6U);
  CHECK(decisions.front().at_secs == 7.0);
  CHECK(scheduler.Flush().empty());
}

TEST_CASE("ArrivalScheduler keeps decision times nondecreasing") {
  correlator::ArrivalScheduler scheduler;

  CHECK(scheduler.Submit(1U, 10.0).empty());
  const std::vector<correlator::Decision> first_decisions =
      scheduler.AdvanceTo(12.0);
  REQUIRE(first_decisions.size() == 1U);

  CHECK(scheduler.Submit(2U, 12.0).empty());
  const std::vector<correlator::Decision> second_decisions = scheduler.Flush();

  REQUIRE(second_decisions.size() == 1U);
  CHECK(first_decisions.front().at_secs <= second_decisions.front().at_secs);
  CHECK(second_decisions.front().at_secs == 14.0);
}

TEST_CASE("ArrivalScheduler enforces a pending-report capacity") {
  correlator::ArrivalScheduler scheduler(2U);

  CHECK(scheduler.Submit(1U, 10.0).empty());
  CHECK(scheduler.Submit(2U, 11.0).empty());

  const std::vector<correlator::Decision> early_decisions =
      scheduler.Submit(3U, 11.0);

  REQUIRE(early_decisions.size() == 1U);
  CHECK(early_decisions.front().report_index == 1U);
  CHECK(early_decisions.front().at_secs == 11.0);

  const std::vector<correlator::Decision> remaining_decisions =
      scheduler.Flush();
  REQUIRE(remaining_decisions.size() == 2U);
  CHECK(remaining_decisions[0].report_index == 2U);
  CHECK(remaining_decisions[1].report_index == 3U);
  CHECK(early_decisions.front().at_secs <= remaining_decisions.front().at_secs);
}

TEST_CASE("TrackStore creates and updates stable tracks") {
  correlator::TrackStore store(2U, 3U, 60.0);
  correlator::Report first_report = MakeValidReport();

  const std::optional<std::string> track_id = store.CreateTrack(first_report);

  REQUIRE(track_id.has_value());
  CHECK(track_id.value() == "T0");
  REQUIRE(store.UpdateTrack(track_id.value(), first_report));
  const correlator::TrackState* track = store.FindTrack(track_id.value());
  REQUIRE(track != nullptr);
  CHECK(track->latest_report.report_index == first_report.report_index);
  CHECK(track->history.size() == 2U);
}

TEST_CASE("TrackStore bounds observation history") {
  correlator::TrackStore store(2U, 2U, 60.0);
  correlator::Report first_report = MakeValidReport();
  REQUIRE(store.CreateTrack(first_report).has_value());
  const std::string track_id = "T0";

  first_report.report_index = 1U;
  REQUIRE(store.UpdateTrack(track_id, first_report));
  first_report.report_index = 2U;
  REQUIRE(store.UpdateTrack(track_id, first_report));

  const correlator::TrackState* track = store.FindTrack(track_id);
  REQUIRE(track != nullptr);
  REQUIRE(track->history.size() == 2U);
  CHECK(track->history.front().report_index == 1U);
  CHECK(track->history.back().report_index == 2U);
}

TEST_CASE("TrackStore retires tracks at the inactivity boundary") {
  correlator::TrackStore store(2U, 2U, 5.0);
  const correlator::Report report = MakeValidReport();
  REQUIRE(store.CreateTrack(report).has_value());

  CHECK(store.RetireStaleTracks(14.999) == 0U);
  CHECK(store.ActiveTrackCount() == 1U);
  CHECK(store.RetireStaleTracks(15.0) == 1U);
  CHECK(store.ActiveTrackCount() == 0U);
}

TEST_CASE("TrackStore never reuses retired track names") {
  correlator::TrackStore store(2U, 2U, 5.0);
  const correlator::Report report = MakeValidReport();
  REQUIRE(store.CreateTrack(report).value() == "T0");
  REQUIRE(store.RetireStaleTracks(15.0) == 1U);

  const std::optional<std::string> replacement = store.CreateTrack(report);

  REQUIRE(replacement.has_value());
  CHECK(replacement.value() == "T1");
}

TEST_CASE("TrackStore refuses new tracks at its active-track limit") {
  correlator::TrackStore store(1U, 2U, 5.0);
  const correlator::Report report = MakeValidReport();
  REQUIRE(store.CreateTrack(report).has_value());

  CHECK_FALSE(store.CreateTrack(report).has_value());
  CHECK(store.ActiveTrackCount() == 1U);
}

TEST_CASE("AssociationScorer selects the nearest feasible track") {
  correlator::Report report = MakeValidReport();
  report.latitude_degs = 47.0001;
  report.longitude_degs = 7.0001;
  const std::vector<correlator::TrackState> tracks = {
      MakeTrack("Tnear", MakeValidReport()),
      MakeTrack("Tfar",
                [&] {
                  correlator::Report far_report = MakeValidReport();
                  far_report.longitude_degs = 7.05;
                  return far_report;
                }()),
  };

  const correlator::AssociationResult result =
      correlator::AssociationScorer(
          correlator::AssociationConfig{.max_distance_m = 10000.0})
          .Evaluate(report, tracks);

  REQUIRE(result.assigned_track_id.has_value());
  CHECK(result.assigned_track_id.value() == "Tnear");
  REQUIRE(result.candidates.size() == 2U);
  CHECK(result.candidates.front().track_id == "Tnear");
}

TEST_CASE(
    "AssociationScorer uses matching identifiers as supporting evidence") {
  correlator::Report report = MakeValidReport();
  report.identifier = "MATCH";
  correlator::Report nearer_report = MakeValidReport();
  nearer_report.longitude_degs = 7.001;
  correlator::Report matching_report = MakeValidReport();
  matching_report.longitude_degs = 7.01;
  matching_report.identifier = "MATCH";
  const std::vector<correlator::TrackState> tracks = {
      MakeTrack("Tnear", nearer_report),
      MakeTrack("Tmatch", matching_report),
  };

  const correlator::AssociationResult result =
      correlator::AssociationScorer(
          correlator::AssociationConfig{.max_distance_m = 10000.0})
          .Evaluate(report, tracks);

  REQUIRE(result.assigned_track_id.has_value());
  CHECK(result.assigned_track_id.value() == "Tmatch");
}

TEST_CASE("AssociationScorer handles missing identifiers without bias") {
  correlator::Report report = MakeValidReport();
  const std::vector<correlator::TrackState> tracks = {
      MakeTrack("Tnear", MakeValidReport()),
  };

  const correlator::AssociationResult result =
      correlator::AssociationScorer().Evaluate(report, tracks);

  REQUIRE(result.assigned_track_id.has_value());
  CHECK(result.assigned_track_id.value() == "Tnear");
}

TEST_CASE("AssociationScorer excludes stale and distant tracks") {
  correlator::Report report = MakeValidReport();
  report.arrival_time_secs = 100.0;
  report.longitude_degs = 8.0;
  const std::vector<correlator::TrackState> tracks = {
      MakeTrack("Tstale", MakeValidReport()),
  };

  const correlator::AssociationResult result =
      correlator::AssociationScorer().Evaluate(report, tracks);

  CHECK(result.candidates.empty());
  CHECK_FALSE(result.assigned_track_id.has_value());
}

TEST_CASE("AssociationScorer rejects an ambiguous tie") {
  const correlator::Report report = MakeValidReport();
  const std::vector<correlator::TrackState> tracks = {
      MakeTrack("T1", MakeValidReport()),
      MakeTrack("T2", MakeValidReport()),
  };

  const correlator::AssociationResult result =
      correlator::AssociationScorer().Evaluate(report, tracks);

  REQUIRE(result.candidates.size() == 2U);
  CHECK(result.candidates[0].score == result.candidates[1].score);
  CHECK_FALSE(result.assigned_track_id.has_value());
}

TEST_CASE("IdentifierMatcher recognizes an identifier from track history") {
  correlator::Report old_report = MakeValidReport();
  old_report.identifier = "OLD";
  correlator::TrackState track = MakeTrack("T1", old_report);
  track.latest_report.identifier = "NEW";
  track.history.push_back(track.latest_report);

  correlator::Report query = MakeValidReport();
  query.identifier = "OLD";

  CHECK(correlator::IdentifierMatcher::Compare(query, track) ==
        correlator::IdentifierRelation::kMatch);
}

TEST_CASE("AssociationScorer retains a track across an identifier change") {
  correlator::Report old_report = MakeValidReport();
  old_report.identifier = "OLD";
  correlator::TrackState track = MakeTrack("T1", old_report);
  track.latest_report.identifier = "NEW";
  track.history.push_back(track.latest_report);

  correlator::Report query = MakeValidReport();
  query.identifier = "OLD";
  const std::vector<correlator::TrackState> tracks = {track};

  const correlator::AssociationResult result =
      correlator::AssociationScorer().Evaluate(query, tracks);

  REQUIRE(result.assigned_track_id.has_value());
  CHECK(result.assigned_track_id.value() == "T1");
}

TEST_CASE("AssociationScorer separates duplicate identifiers by position") {
  correlator::Report query = MakeValidReport();
  query.identifier = "DUPLICATE";
  correlator::Report near_report = MakeValidReport();
  near_report.identifier = "DUPLICATE";
  correlator::Report far_report = near_report;
  far_report.longitude_degs = 7.05;
  const std::vector<correlator::TrackState> tracks = {
      MakeTrack("Tnear", near_report),
      MakeTrack("Tfar", far_report),
  };

  const correlator::AssociationResult result =
      correlator::AssociationScorer().Evaluate(query, tracks);

  REQUIRE(result.assigned_track_id.has_value());
  CHECK(result.assigned_track_id.value() == "Tnear");
}

TEST_CASE(
    "IdentifierMatcher distinguishes missing and conflicting identifiers") {
  correlator::Report report = MakeValidReport();
  correlator::TrackState track = MakeTrack("T1", report);

  correlator::Report missing_identifier = report;
  missing_identifier.identifier = std::nullopt;
  CHECK(correlator::IdentifierMatcher::Compare(missing_identifier, track) ==
        correlator::IdentifierRelation::kMissing);

  report.identifier = "OTHER";
  track.latest_report.identifier = "KNOWN";
  track.history.front().identifier = "KNOWN";
  CHECK(correlator::IdentifierMatcher::Compare(report, track) ==
        correlator::IdentifierRelation::kConflict);
}

TEST_CASE("AssociationScorer rejects contradictory identity and position") {
  correlator::Report report = MakeValidReport();
  report.identifier = "KNOWN";
  report.longitude_degs = 7.1001;
  correlator::Report matching_report = MakeValidReport();
  matching_report.identifier = "KNOWN";
  correlator::Report conflicting_report = MakeValidReport();
  conflicting_report.identifier = "OTHER";
  conflicting_report.longitude_degs = 7.1001;
  const std::vector<correlator::TrackState> tracks = {
      MakeTrack("Tknown", matching_report),
      MakeTrack("Tother", conflicting_report),
  };

  const correlator::AssociationResult result =
      correlator::AssociationScorer().Evaluate(report, tracks);

  CHECK(result.candidates.empty());
  CHECK(result.rejected_for_identifier_conflict);
  CHECK_FALSE(result.assigned_track_id.has_value());
}

TEST_CASE("AnswerWriter serializes assigned and rejected answers") {
  std::ostringstream output;
  correlator::AnswerWriter::Write(
      output,
      correlator::Answer{
          .report_index = 3U, .track_id = std::string("T7"), .at_secs = 12.5});
  correlator::AnswerWriter::Write(
      output,
      correlator::Answer{
          .report_index = 4U, .track_id = std::nullopt, .at_secs = 13.0});

  const std::string serialized = output.str();
  const std::size_t separator = serialized.find('\n');
  const nlohmann::json assigned =
      nlohmann::json::parse(serialized.substr(0, separator));
  const nlohmann::json rejected =
      nlohmann::json::parse(serialized.substr(separator + 1));
  CHECK(assigned.at("report") == 3U);
  CHECK(assigned.at("track") == "T7");
  CHECK(assigned.at("at") == 12.5);
  CHECK(rejected.at("report") == 4U);
  CHECK(rejected.at("track").is_null());
  CHECK(rejected.at("at") == 13.0);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// readability-identifier-length, bugprone-easily-swappable-parameters)
