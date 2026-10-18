#include "report_parser.h"

#include <cmath>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace correlator {
namespace {

using Json = nlohmann::json;

ParseResult Failure(ParseErrorCode code, std::string message) {
  return ParseResult{
      .report = std::nullopt,
      .error = ParseError{.code = code, .message = std::move(message)}};
}

bool HasNumber(const Json& object, std::string_view name) {
  return object.contains(name) && object.at(name).is_number();
}

bool HasString(const Json& object, std::string_view name) {
  return object.contains(name) && object.at(name).is_string();
}

bool IsFinite(double value) { return std::isfinite(value); }

}  // namespace

ParseResult ReportParser::Parse(std::string_view line,
                                std::size_t report_index) {
  Json object;
  try {
    object = Json::parse(line);
  } catch (const Json::parse_error& error) {
    return Failure(ParseErrorCode::kMalformedJson, error.what());
  }

  if (!object.is_object()) {
    return Failure(ParseErrorCode::kInvalidType,
                   "Report must be a JSON object");
  }

  constexpr std::string_view kRequiredNumberFields[] = {"arr", "t", "lat",
                                                        "lon"};
  for (const std::string_view field : kRequiredNumberFields) {
    if (!object.contains(field)) {
      return Failure(ParseErrorCode::kMissingField,
                     "Missing required field: " + std::string(field));
    }
    if (!object.at(field).is_number()) {
      return Failure(ParseErrorCode::kInvalidType,
                     "Field must be numeric: " + std::string(field));
    }
    if (!IsFinite(object.at(field).get<double>())) {
      return Failure(ParseErrorCode::kInvalidValue,
                     "Field must be finite: " + std::string(field));
    }
  }

  if (!object.contains("rx")) {
    return Failure(ParseErrorCode::kMissingField, "Missing required field: rx");
  }
  if (!HasString(object, "rx")) {
    return Failure(ParseErrorCode::kInvalidType, "Field must be a string: rx");
  }

  std::optional<std::string> identifier;
  if (object.contains("id") && !object.at("id").is_null()) {
    if (!object.at("id").is_string()) {
      return Failure(ParseErrorCode::kInvalidType,
                     "Field must be a string: id");
    }
    identifier = object.at("id").get<std::string>();
  }

  std::optional<double> altitude_m;
  if (object.contains("alt") && !object.at("alt").is_null()) {
    if (!HasNumber(object, "alt")) {
      return Failure(ParseErrorCode::kInvalidType,
                     "Field must be numeric: alt");
    }
    altitude_m = object.at("alt").get<double>();
    if (!IsFinite(altitude_m.value())) {
      return Failure(ParseErrorCode::kInvalidValue,
                     "Field must be finite: alt");
    }
  }

  return ParseResult{
      .report =
          Report{
              .report_index = report_index,
              .arrival_time_secs = object.at("arr").get<double>(),
              .receiver = object.at("rx").get<std::string>(),
              .receiver_time_secs = object.at("t").get<double>(),
              .identifier = std::move(identifier),
              .latitude_degs = object.at("lat").get<double>(),
              .longitude_degs = object.at("lon").get<double>(),
              .altitude_m = altitude_m,
          },
      .error = std::nullopt,
  };
}

}  // namespace correlator
