#ifndef INVOLI_TRACK_CORRELATOR_SRC_TRACK_STORE_H_
#define INVOLI_TRACK_CORRELATOR_SRC_TRACK_STORE_H_

#include <cstddef>
#include <deque>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "report.h"

namespace correlator {

inline constexpr std::size_t kDefaultMaxActiveTracks = 4096U;
inline constexpr std::size_t kDefaultMaxTrackHistory = 32U;
inline constexpr double kDefaultTrackRetirementSecs = 60.0;

struct TrackState {
  std::string track_id;
  Report latest_report;
  std::deque<Report> history;
};

// Owns bounded active-track state and assigns stable process-lifetime names.
class TrackStore {
 public:
  explicit TrackStore(
      std::size_t max_active_tracks = kDefaultMaxActiveTracks,
      std::size_t max_track_history = kDefaultMaxTrackHistory,
      double track_retirement_secs = kDefaultTrackRetirementSecs);

  // Creates a track, or returns null when the active-track capacity is full.
  [[nodiscard]] std::optional<std::string> CreateTrack(const Report& report);

  // Appends an accepted observation to an existing track.
  [[nodiscard]] bool UpdateTrack(std::string_view track_id,
                                 const Report& report);

  // Returns the active track, or null when no track has that name.
  [[nodiscard]] const TrackState* FindTrack(std::string_view track_id) const;

  // Returns a bounded snapshot for association evaluation.
  [[nodiscard]] std::vector<TrackState> Snapshot() const;

  // Removes tracks inactive for at least the configured retirement interval.
  [[nodiscard]] std::size_t RetireStaleTracks(double current_time_secs);

  [[nodiscard]] std::size_t ActiveTrackCount() const noexcept {
    return tracks_.size();
  }

 private:
  void AppendObservation(TrackState& track, const Report& report);

  std::unordered_map<std::string, TrackState> tracks_;
  std::size_t next_track_number_ = 0U;
  std::size_t max_active_tracks_;
  std::size_t max_track_history_;
  double track_retirement_secs_;
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_TRACK_STORE_H_
