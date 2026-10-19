#include "geo_utils.h"

#include <cmath>
#include <numbers>

namespace correlator {
namespace {

constexpr double kMetersPerDegree = 111320.0;
constexpr double kDegreesToRadians = std::numbers::pi / 180.0;

}  // namespace

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
double CalculateHorizontalDistanceM(double latitude_degs, double longitude_degs,
                                    double other_latitude_degs,
                                    double other_longitude_degs) {
  const double reference_latitude_rads =
      (latitude_degs + other_latitude_degs) * 0.5 * kDegreesToRadians;
  const double north_m =
      (latitude_degs - other_latitude_degs) * kMetersPerDegree;
  const double east_m = (longitude_degs - other_longitude_degs) *
                        kMetersPerDegree * std::cos(reference_latitude_rads);
  return std::hypot(north_m, east_m);
}
// NOLINTEND(bugprone-easily-swappable-parameters)

}  // namespace correlator
