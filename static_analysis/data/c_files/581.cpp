uint32 PluginInstance::ScheduleTimer(uint32 interval,
NPBool repeat,
void (*func)(NPP id, uint32 timer_id)) {
// Use next timer id.
uint32 timer_id;
timer_id = next_timer_id_;
++next_timer_id_;
DCHECK(next_timer_id_ != 0);

// Record timer interval and repeat.
TimerInfo info;
info.interval = interval;
info.repeat = repeat;
timers_[timer_id] = info;

// Schedule the callback.
  message_loop_->PostDelayedTask(FROM_HERE,
                                 NewRunnableMethod(this,
                                                   &PluginInstance::OnTimerCall,
                                                   func,
                                                   npp_,
                                                   timer_id),
                                 interval);
return timer_id;
}
