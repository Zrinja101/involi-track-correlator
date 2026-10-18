#ifndef INVOLI_TRACK_CORRELATOR_SRC_ARRIVAL_SCHEDULER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_ARRIVAL_SCHEDULER_H_

#include <cstddef>
#include <vector>

namespace correlator {

inline constexpr double kDecisionWindowSecs = 2.0;

struct Decision {
  std::size_t report_index;
  double at_secs;
};

// Schedules report decisions against the input arrival-time clock.
class ArrivalScheduler {
 public:
  // Adds a report and returns any reports that became due at its arrival time.
  [[nodiscard]] std::vector<Decision> Submit(std::size_t report_index,
                                             double arrival_time_secs);

  // Advances the arrival clock and returns reports whose deadlines are due.
  [[nodiscard]] std::vector<Decision> AdvanceTo(double arrival_time_secs);

  // Finalizes every pending report at its deadline in deterministic order.
  [[nodiscard]] std::vector<Decision> Flush();

 private:
  struct PendingDecision {
    std::size_t report_index;
    double deadline_secs;
  };

  [[nodiscard]] std::vector<Decision> FinalizeDue(bool finalize_all);

  std::vector<PendingDecision> pending_;
  double current_time_secs_ = 0.0;
  double last_at_secs_ = 0.0;
  bool has_current_time_ = false;
  bool has_last_at_ = false;
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_ARRIVAL_SCHEDULER_H_
