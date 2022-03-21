SchedulerHelper::SchedulerHelper(
scoped_refptr<NestableSingleThreadTaskRunner> main_task_runner,
const char* tracing_category,
const char* disabled_by_default_tracing_category,
size_t total_task_queue_count)
: task_queue_selector_(new PrioritizingTaskQueueSelector()),
task_queue_manager_(
new TaskQueueManager(total_task_queue_count,
main_task_runner,
task_queue_selector_.get(),
disabled_by_default_tracing_category)),
quiescence_monitored_task_queue_mask_(
((1ull << total_task_queue_count) - 1ull) &
~(1ull << QueueId::CONTROL_TASK_QUEUE) &
~(1ull << QueueId::CONTROL_TASK_AFTER_WAKEUP_QUEUE)),
control_task_runner_(
task_queue_manager_->TaskRunnerForQueue(QueueId::CONTROL_TASK_QUEUE)),
control_after_wakeup_task_runner_(task_queue_manager_->TaskRunnerForQueue(
QueueId::CONTROL_TASK_AFTER_WAKEUP_QUEUE)),
default_task_runner_(
task_queue_manager_->TaskRunnerForQueue(QueueId::DEFAULT_TASK_QUEUE)),
      time_source_(new TimeSource),
tracing_category_(tracing_category),
disabled_by_default_tracing_category_(
disabled_by_default_tracing_category) {
DCHECK_GE(total_task_queue_count,
static_cast<size_t>(QueueId::TASK_QUEUE_COUNT));
task_queue_selector_->SetQueuePriority(
QueueId::CONTROL_TASK_QUEUE,
PrioritizingTaskQueueSelector::CONTROL_PRIORITY);
task_queue_manager_->SetWakeupPolicy(
QueueId::CONTROL_TASK_QUEUE,
TaskQueueManager::WakeupPolicy::DONT_WAKE_OTHER_QUEUES);

task_queue_selector_->SetQueuePriority(
QueueId::CONTROL_TASK_AFTER_WAKEUP_QUEUE,
PrioritizingTaskQueueSelector::CONTROL_PRIORITY);
task_queue_manager_->SetPumpPolicy(
QueueId::CONTROL_TASK_AFTER_WAKEUP_QUEUE,
TaskQueueManager::PumpPolicy::AFTER_WAKEUP);
task_queue_manager_->SetWakeupPolicy(
QueueId::CONTROL_TASK_AFTER_WAKEUP_QUEUE,
TaskQueueManager::WakeupPolicy::DONT_WAKE_OTHER_QUEUES);

for (size_t i = 0; i < TASK_QUEUE_COUNT; i++) {
task_queue_manager_->SetQueueName(
i, TaskQueueIdToString(static_cast<QueueId>(i)));
}

// TODO(skyostil): Increase this to 4 (crbug.com/444764).
task_queue_manager_->SetWorkBatchSize(1);
}
