#include "runtime_config.h"

#include <cmath>
#include <fstream>
#include <initializer_list>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace correlator {
namespace {

using Json = nlohmann::json;

[[nodiscard]] double ReadDouble(const Json& object, const char* name,
                                double default_value) {
  if (!object.contains(name)) {
    return default_value;
  }
  if (!object.at(name).is_number()) {
    throw std::runtime_error(std::string("Configuration field '") + name +
                             "' must be a number");
  }
  return object.at(name).get<double>();
}

[[nodiscard]] std::size_t ReadSize(const Json& object, const char* name,
                                   std::size_t default_value) {
  if (!object.contains(name)) {
    return default_value;
  }
  if (!object.at(name).is_number_unsigned()) {
    throw std::runtime_error(std::string("Configuration field '") + name +
                             "' must be a positive integer");
  }
  return object.at(name).get<std::size_t>();
}

void RejectUnknownFields(const Json& object,
                         std::initializer_list<const char*> allowed,
                         const char* section) {
  for (const auto& [name, value] : object.items()) {
    bool known = false;
    for (const char* allowed_name : allowed) {
      known = known || name == allowed_name;
    }
    if (!known) {
      throw std::runtime_error(std::string("Unknown configuration field '") +
                               section + "." + name + "'");
    }
  }
}

void ValidateRuntimeConfig(const RuntimeConfig& config) {
  if (config.max_pending_reports == 0U ||
      config.max_pending_reports > 1'000'000U ||
      config.max_active_tracks == 0U || config.max_active_tracks > 1'000'000U ||
      config.max_track_history == 0U || config.max_track_history > 10'000U) {
    throw std::runtime_error("Configuration capacity is outside its bounds");
  }
  if (!std::isfinite(config.decision_window_secs) ||
      config.decision_window_secs <= 0.0 ||
      config.decision_window_secs > kDecisionWindowSecs ||
      !std::isfinite(config.track_retirement_secs) ||
      config.track_retirement_secs <= 0.0 ||
      config.track_retirement_secs > 86'400.0) {
    throw std::runtime_error("Configuration time limit is outside its bounds");
  }
  try {
    static_cast<void>(AssociationScorer(config.association));
    static_cast<void>(PlausibilityValidator(config.plausibility));
    static_cast<void>(TrackStore(config.max_active_tracks,
                                 config.max_track_history,
                                 config.track_retirement_secs));
    static_cast<void>(ArrivalScheduler(config.max_pending_reports));
  } catch (const std::invalid_argument& error) {
    throw std::runtime_error(error.what());
  }
}

}  // namespace

RuntimeConfig DefaultRuntimeConfig() {
  RuntimeConfig config;
  ValidateRuntimeConfig(config);
  return config;
}

RuntimeConfig LoadRuntimeConfig(const std::string& path) {
  std::ifstream input(path);
  if (!input) {
    throw std::runtime_error("Cannot open configuration file '" + path + "'");
  }

  Json root;
  try {
    input >> root;
  } catch (const Json::exception& error) {
    throw std::runtime_error("Cannot parse configuration file '" + path +
                             "': " + error.what());
  }
  if (!root.is_object()) {
    throw std::runtime_error("Configuration root must be an object");
  }
  RejectUnknownFields(
      root, {"association", "plausibility", "scheduler", "track_store"},
      "root");

  RuntimeConfig config;
  const Json empty = Json::object();
  const Json& association =
      root.contains("association") ? root.at("association") : empty;
  const Json& plausibility =
      root.contains("plausibility") ? root.at("plausibility") : empty;
  const Json& scheduler =
      root.contains("scheduler") ? root.at("scheduler") : empty;
  const Json& track_store =
      root.contains("track_store") ? root.at("track_store") : empty;
  for (const Json* section :
       {&association, &plausibility, &scheduler, &track_store}) {
    if (!section->is_object()) {
      throw std::runtime_error("Configuration sections must be objects");
    }
  }
  RejectUnknownFields(
      association,
      {"max_track_age_secs", "max_distance_m", "min_score", "min_score_margin",
       "distance_weight", "age_weight", "identifier_weight", "altitude_weight",
       "minimum_velocity_interval_secs", "maximum_prediction_speed_mps",
       "altitude_evidence_range_m"},
      "association");
  RejectUnknownFields(plausibility,
                      {"max_horizontal_speed_mps", "max_vertical_speed_mps"},
                      "plausibility");
  RejectUnknownFields(
      scheduler, {"max_pending_reports", "decision_window_secs"}, "scheduler");
  RejectUnknownFields(
      track_store,
      {"max_active_tracks", "max_track_history", "track_retirement_secs"},
      "track_store");

  config.association.max_track_age_secs = ReadDouble(
      association, "max_track_age_secs", config.association.max_track_age_secs);
  config.association.max_distance_m = ReadDouble(
      association, "max_distance_m", config.association.max_distance_m);
  config.association.min_score =
      ReadDouble(association, "min_score", config.association.min_score);
  config.association.min_score_margin = ReadDouble(
      association, "min_score_margin", config.association.min_score_margin);
  config.association.distance_weight = ReadDouble(
      association, "distance_weight", config.association.distance_weight);
  config.association.age_weight =
      ReadDouble(association, "age_weight", config.association.age_weight);
  config.association.identifier_weight = ReadDouble(
      association, "identifier_weight", config.association.identifier_weight);
  config.association.altitude_weight = ReadDouble(
      association, "altitude_weight", config.association.altitude_weight);
  config.association.minimum_velocity_interval_secs =
      ReadDouble(association, "minimum_velocity_interval_secs",
                 config.association.minimum_velocity_interval_secs);
  config.association.maximum_prediction_speed_mps =
      ReadDouble(association, "maximum_prediction_speed_mps",
                 config.association.maximum_prediction_speed_mps);
  config.association.altitude_evidence_range_m =
      ReadDouble(association, "altitude_evidence_range_m",
                 config.association.altitude_evidence_range_m);
  config.plausibility.max_horizontal_speed_mps =
      ReadDouble(plausibility, "max_horizontal_speed_mps",
                 config.plausibility.max_horizontal_speed_mps);
  config.plausibility.max_vertical_speed_mps =
      ReadDouble(plausibility, "max_vertical_speed_mps",
                 config.plausibility.max_vertical_speed_mps);
  config.max_pending_reports =
      ReadSize(scheduler, "max_pending_reports", config.max_pending_reports);
  config.decision_window_secs = ReadDouble(scheduler, "decision_window_secs",
                                           config.decision_window_secs);
  config.max_active_tracks =
      ReadSize(track_store, "max_active_tracks", config.max_active_tracks);
  config.max_track_history =
      ReadSize(track_store, "max_track_history", config.max_track_history);
  config.track_retirement_secs = ReadDouble(
      track_store, "track_retirement_secs", config.track_retirement_secs);
  ValidateRuntimeConfig(config);
  return config;
}

}  // namespace correlator