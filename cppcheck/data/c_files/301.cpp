static irqreturn_t armv7pmu_handle_irq(int irq_num, void *dev)
{
unsigned long pmnc;
struct perf_sample_data data;
struct cpu_hw_events *cpuc;
struct pt_regs *regs;
int idx;

/*
* Get and reset the IRQ flags
*/
pmnc = armv7_pmnc_getreset_flags();

/*
* Did an overflow occur?
*/
if (!armv7_pmnc_has_overflowed(pmnc))
return IRQ_NONE;

/*
* Handle the counter(s) overflow(s)
*/
regs = get_irq_regs();

perf_sample_data_init(&data, 0);

cpuc = &__get_cpu_var(cpu_hw_events);
for (idx = 0; idx <= armpmu->num_events; ++idx) {
struct perf_event *event = cpuc->events[idx];
struct hw_perf_event *hwc;

if (!test_bit(idx, cpuc->active_mask))
continue;

/*
* We have a single interrupt for all counters. Check that
* each counter has overflowed before we process it.
*/
if (!armv7_pmnc_counter_has_overflowed(pmnc, idx))
continue;

hwc = &event->hw;
armpmu_event_update(event, hwc, idx, 1);
data.period = event->hw.last_period;
if (!armpmu_event_set_period(event, hwc, idx))
continue;

		if (perf_event_overflow(event, 0, &data, regs))
armpmu->disable(hwc, idx);
}

/*
* Handle the pending perf events.
*
* Note: this call *must* be run with interrupts disabled. For
* platforms that can have the PMU interrupts raised as an NMI, this
* will not work.
*/
irq_work_run();

return IRQ_HANDLED;
}
