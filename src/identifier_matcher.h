#ifndef INVOLI_TRACK_CORRELATOR_SRC_IDENTIFIER_MATCHER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_IDENTIFIER_MATCHER_H_

#include "report.h"
#include "track_store.h"

namespace correlator {

enum class IdentifierRelation {
  kMissing,
  kMatch,
  kConflict,
};

// Compares a report identifier with all identifiers retained by a track.
class IdentifierMatcher {
 public:
  [[nodiscard]] static IdentifierRelation Compare(const Report& report,
                                                  const TrackState& track);
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_IDENTIFIER_MATCHER_H_
