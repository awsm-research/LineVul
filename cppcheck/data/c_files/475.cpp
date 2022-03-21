static void perf_swevent_overflow(struct perf_event *event, u64 overflow,
				    int nmi, struct perf_sample_data *data,
struct pt_regs *regs)
{
struct hw_perf_event *hwc = &event->hw;
int throttle = 0;

data->period = event->hw.last_period;
if (!overflow)
overflow = perf_swevent_set_period(event);

if (hwc->interrupts == MAX_INTERRUPTS)
return;

for (; overflow; overflow--) {
		if (__perf_event_overflow(event, nmi, throttle,
data, regs)) {
/*
* We inhibit the overflow from happening when
* hwc->interrupts == MAX_INTERRUPTS.
*/
break;
}
throttle = 1;
}
}
