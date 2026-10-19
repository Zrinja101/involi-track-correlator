#ifndef INVOLI_TRACK_CORRELATOR_SRC_GEO_UTILS_H_
#define INVOLI_TRACK_CORRELATOR_SRC_GEO_UTILS_H_

namespace correlator {

/// Calculates short-range horizontal distance using a local WGS84
/// approximation.
///
/// Latitude and longitude arguments are in degrees and the returned distance is
/// in metres. This utility is intended for the local-area trajectories used by
/// the correlator; it does not perform coordinate validation.
// NOLINTBEGIN(bugprone-easily-swappable-parameters)
[[nodiscard]] double CalculateHorizontalDistanceM(double latitude_degs,
                                                  double longitude_degs,
                                                  double other_latitude_degs,
                                                  double other_longitude_degs);
// NOLINTEND(bugprone-easily-swappable-parameters)

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_GEO_UTILS_H_
