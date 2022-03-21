bool MessageLoop::DoDelayedWork(TimeTicks* next_delayed_work_time) {
  if (!nestable_tasks_allowed_ ||
!SweepDelayedWorkQueueAndReturnTrueIfStillHasWork()) {
recent_time_ = *next_delayed_work_time = TimeTicks();
return false;
}

// When we "fall behind", there will be a lot of tasks in the delayed work
// queue that are ready to run.  To increase efficiency when we fall behind,
// we will only call Time::Now() intermittently, and then process all tasks
// that are ready to run before calling it again.  As a result, the more we
// fall behind (and have a lot of ready-to-run delayed tasks), the more
// efficient we'll be at handling the tasks.

TimeTicks next_run_time = delayed_work_queue_.top().delayed_run_time;
if (next_run_time > recent_time_) {
recent_time_ = TimeTicks::Now();  // Get a better view of Now();
if (next_run_time > recent_time_) {
*next_delayed_work_time = next_run_time;
return false;
}
}

PendingTask pending_task =
std::move(const_cast<PendingTask&>(delayed_work_queue_.top()));
delayed_work_queue_.pop();

if (SweepDelayedWorkQueueAndReturnTrueIfStillHasWork())
*next_delayed_work_time = delayed_work_queue_.top().delayed_run_time;

return DeferOrRunPendingTask(std::move(pending_task));
}
