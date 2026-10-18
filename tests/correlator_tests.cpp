#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <vector>

#include "arrival_scheduler.h"
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

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// readability-identifier-length, bugprone-easily-swappable-parameters)
