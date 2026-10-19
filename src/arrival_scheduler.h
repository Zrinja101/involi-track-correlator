#ifndef INVOLI_TRACK_CORRELATOR_SRC_ARRIVAL_SCHEDULER_H_
#define INVOLI_TRACK_CORRELATOR_SRC_ARRIVAL_SCHEDULER_H_

#include <cstddef>
#include <vector>

namespace correlator {

/// Maximum permitted delay between report arrival and decision, in seconds.
inline constexpr double kDecisionWindowSecs = 2.0;
/// Default hard limit for reports retained while awaiting decisions.
inline constexpr std::size_t kDefaultMaxPendingReports = 4096U;

/// A report index and its simulated arrival-clock decision time.
struct Decision {
  /// Zero-based input report index.
  std::size_t report_index;
  /// Simulated decision time on the arrival clock, in seconds.
  double at_secs;
};

// Schedules report decisions against the input arrival-time clock.
class ArrivalScheduler {
 public:
  /// Constructs a scheduler with a finite pending-report capacity.
  ///
  /// A zero capacity is invalid and causes std::invalid_argument to be thrown.
  explicit ArrivalScheduler(
      std::size_t max_pending_reports = kDefaultMaxPendingReports,
      double decision_window_secs = kDecisionWindowSecs);

  // Adds a report and returns any reports that became due at its arrival time.
  /// Adds a report and returns decisions due at its arrival time.
  ///
  /// `arrival_time_secs` must not move the input clock backwards. If capacity
  /// is reached, the oldest pending report is finalized conservatively.
  [[nodiscard]] std::vector<Decision> Submit(std::size_t report_index,
                                             double arrival_time_secs);

  // Advances the arrival clock and returns reports whose deadlines are due.
  /// Advances the simulated arrival clock and finalizes due reports.
  ///
  /// Calling this method with an earlier time leaves the clock unchanged.
  [[nodiscard]] std::vector<Decision> AdvanceTo(double arrival_time_secs);

  // Finalizes every pending report at its deadline in deterministic order.
  /// Finalizes all remaining reports at deterministic deadline times.
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
  std::size_t max_pending_reports_;
  double decision_window_secs_;
};

}  // namespace correlator

#endif  // INVOLI_TRACK_CORRELATOR_SRC_ARRIVAL_SCHEDULER_H_
