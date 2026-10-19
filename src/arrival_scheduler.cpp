#include "arrival_scheduler.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace correlator {

ArrivalScheduler::ArrivalScheduler(std::size_t max_pending_reports,
                                   double decision_window_secs)
    : max_pending_reports_(max_pending_reports),
      decision_window_secs_(decision_window_secs) {
  if (max_pending_reports == 0U || !std::isfinite(decision_window_secs) ||
      decision_window_secs <= 0.0) {
    throw std::invalid_argument("Scheduler configuration is invalid");
  }
}

std::vector<Decision> ArrivalScheduler::Submit(std::size_t report_index,
                                               double arrival_time_secs) {
  std::vector<Decision> decisions = AdvanceTo(arrival_time_secs);
  if (pending_.size() >= max_pending_reports_) {
    const auto oldest = std::min_element(
        pending_.begin(), pending_.end(),
        [](const PendingDecision& first, const PendingDecision& second) {
          if (first.deadline_secs != second.deadline_secs) {
            return first.deadline_secs < second.deadline_secs;
          }
          return first.report_index < second.report_index;
        });
    const PendingDecision pending = *oldest;
    pending_.erase(oldest);
    const double at_secs = has_last_at_
                               ? std::max(last_at_secs_, current_time_secs_)
                               : current_time_secs_;
    decisions.push_back(
        Decision{.report_index = pending.report_index, .at_secs = at_secs});
    last_at_secs_ = at_secs;
    has_last_at_ = true;
  }
  pending_.push_back(PendingDecision{
      .report_index = report_index,
      .deadline_secs = arrival_time_secs + decision_window_secs_,
  });
  return decisions;
}

std::vector<Decision> ArrivalScheduler::AdvanceTo(double arrival_time_secs) {
  if (!has_current_time_ || arrival_time_secs > current_time_secs_) {
    current_time_secs_ = arrival_time_secs;
    has_current_time_ = true;
  }
  return FinalizeDue(false);
}

std::vector<Decision> ArrivalScheduler::Flush() { return FinalizeDue(true); }

std::vector<Decision> ArrivalScheduler::FinalizeDue(bool finalize_all) {
  std::vector<PendingDecision> due;
  std::vector<PendingDecision> remaining;
  due.reserve(pending_.size());
  remaining.reserve(pending_.size());

  for (const PendingDecision& pending : pending_) {
    if (finalize_all || pending.deadline_secs <= current_time_secs_) {
      due.push_back(pending);
    } else {
      remaining.push_back(pending);
    }
  }
  pending_ = std::move(remaining);
  std::sort(due.begin(), due.end(),
            [](const PendingDecision& first, const PendingDecision& second) {
              if (first.deadline_secs != second.deadline_secs) {
                return first.deadline_secs < second.deadline_secs;
              }
              return first.report_index < second.report_index;
            });

  std::vector<Decision> decisions;
  decisions.reserve(due.size());
  for (const PendingDecision& pending : due) {
    const double at_secs = has_last_at_
                               ? std::max(last_at_secs_, pending.deadline_secs)
                               : pending.deadline_secs;
    decisions.push_back(
        Decision{.report_index = pending.report_index, .at_secs = at_secs});
    last_at_secs_ = at_secs;
    has_last_at_ = true;
  }
  return decisions;
}

}  // namespace correlator
