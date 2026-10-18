#include "track_store.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace correlator {

TrackStore::TrackStore(std::size_t max_active_tracks,
                       std::size_t max_track_history,
                       double track_retirement_secs)
    : max_active_tracks_(max_active_tracks),
      max_track_history_(max_track_history),
      track_retirement_secs_(track_retirement_secs) {
  if (max_active_tracks == 0U || max_track_history == 0U ||
      !std::isfinite(track_retirement_secs) || track_retirement_secs <= 0.0) {
    throw std::invalid_argument(
        "Track-store limits must be positive and finite");
  }
}

std::optional<std::string> TrackStore::CreateTrack(const Report& report) {
  if (tracks_.size() >= max_active_tracks_) {
    return std::nullopt;
  }

  std::string track_id;
  do {
    track_id = "T" + std::to_string(next_track_number_++);
  } while (tracks_.contains(track_id));

  TrackState track{
      .track_id = track_id,
      .latest_report = report,
      .history = {},
  };
  track.history.push_back(report);
  tracks_.emplace(track_id, std::move(track));
  return track_id;
}

bool TrackStore::UpdateTrack(std::string_view track_id, const Report& report) {
  const auto track_iterator = tracks_.find(std::string(track_id));
  if (track_iterator == tracks_.end()) {
    return false;
  }

  AppendObservation(track_iterator->second, report);
  return true;
}

const TrackState* TrackStore::FindTrack(std::string_view track_id) const {
  const auto track_iterator = tracks_.find(std::string(track_id));
  return track_iterator == tracks_.end() ? nullptr : &track_iterator->second;
}

std::size_t TrackStore::RetireStaleTracks(double current_time_secs) {
  std::size_t retired_count = 0U;
  for (auto track_iterator = tracks_.begin();
       track_iterator != tracks_.end();) {
    const double last_arrival_time_secs =
        track_iterator->second.latest_report.arrival_time_secs;
    if (current_time_secs - last_arrival_time_secs >= track_retirement_secs_) {
      track_iterator = tracks_.erase(track_iterator);
      ++retired_count;
    } else {
      ++track_iterator;
    }
  }
  return retired_count;
}

void TrackStore::AppendObservation(TrackState& track, const Report& report) {
  track.latest_report = report;
  track.history.push_back(report);
  while (track.history.size() > max_track_history_) {
    track.history.pop_front();
  }
}

}  // namespace correlator
