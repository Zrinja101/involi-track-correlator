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

/// Default maximum number of concurrently active tracks.
inline constexpr std::size_t kDefaultMaxActiveTracks = 4096U;
/// Default number of observations retained per track.
inline constexpr std::size_t kDefaultMaxTrackHistory = 32U;
/// Default inactivity interval before retirement, in seconds.
inline constexpr double kDefaultTrackRetirementSecs = 60.0;

/// Bounded state and recent observation history for one track.
struct TrackState {
  /// Stable process-lifetime track name.
  std::string track_id;
  /// Most recently accepted observation.
  Report latest_report;
  /// Chronological bounded history, including latest_report.
  std::deque<Report> history;
};

// Owns bounded active-track state and assigns stable process-lifetime names.
class TrackStore {
 public:
  /// Constructs a store with explicit active-track, history, and retirement
  /// limits.
  ///
  /// All limits must be positive and the retirement interval must be finite.
  explicit TrackStore(
      std::size_t max_active_tracks = kDefaultMaxActiveTracks,
      std::size_t max_track_history = kDefaultMaxTrackHistory,
      double track_retirement_secs = kDefaultTrackRetirementSecs);

  /// Creates a track, or returns null when active-track capacity is full.
  ///
  /// Names increase monotonically and are never reused after retirement.
  [[nodiscard]] std::optional<std::string> CreateTrack(const Report& report);

  /// Appends an accepted observation and evicts the oldest history entry if
  /// needed.
  [[nodiscard]] bool UpdateTrack(std::string_view track_id,
                                 const Report& report);

  /// Returns the active track, or null when no track has that name.
  [[nodiscard]] const TrackState* FindTrack(std::string_view track_id) const;

  /// Returns non-owning references to the currently active tracks.
  ///
  /// The returned pointers remain valid until the store is mutated or
  /// destroyed. The snapshot itself is a bounded container, but does not copy
  /// each track's observation history.
  [[nodiscard]] std::vector<const TrackState*> Snapshot() const;

  /// Removes tracks inactive for at least the configured retirement interval.
  ///
  /// Returns the number of retired tracks.
  [[nodiscard]] std::size_t RetireStaleTracks(double current_time_secs);

  /// Returns the number of currently active tracks.
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
