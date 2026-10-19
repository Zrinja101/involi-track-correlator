#include "identifier_matcher.h"

namespace correlator {

IdentifierRelation IdentifierMatcher::Compare(const Report& report,
                                              const TrackState& track) {
  if (!report.identifier.has_value()) {
    return IdentifierRelation::kMissing;
  }

  bool has_track_identifier = false;
  for (const Report& historical_report : track.history) {
    if (!historical_report.identifier.has_value()) {
      continue;
    }
    has_track_identifier = true;
    if (historical_report.identifier == report.identifier) {
      return IdentifierRelation::kMatch;
    }
  }
  return has_track_identifier ? IdentifierRelation::kConflict
                              : IdentifierRelation::kMissing;
}

}  // namespace correlator
