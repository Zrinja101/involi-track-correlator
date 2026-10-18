#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <optional>
#include <string>

#include "report_parser.h"

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

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers,
// readability-identifier-length, bugprone-easily-swappable-parameters)
