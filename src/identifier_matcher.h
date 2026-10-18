#ifndef INVOLI_TRACK_CORRELATOR_SRC_IDENTIFIER_MATCHER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_IDENTIFIER_MATCHER_H_

#include "report.h"
#include "track_store.h"

namespace correlator {

/// Relationship between a report identifier and a track's retained identifiers.
enum class IdentifierRelation {
  kMissing,
  kMatch,
  kConflict,
};

// Compares a report identifier with all identifiers retained by a track.
class IdentifierMatcher {
 public:
  /// Compares the report identifier with every identifier in track history.
  ///
  /// A historical match supports continuity across an identifier change;
  /// missing identifiers are neutral and do not create a conflict.
  [[nodiscard]] static IdentifierRelation Compare(const Report& report,
                                                  const TrackState& track);
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_IDENTIFIER_MATCHER_H_
