static void sgi_timer_get(struct k_itimer *timr, struct itimerspec *cur_setting)
{

if (timr->it.mmtimer.clock == TIMER_OFF) {
cur_setting->it_interval.tv_nsec = 0;
cur_setting->it_interval.tv_sec = 0;
cur_setting->it_value.tv_nsec = 0;
cur_setting->it_value.tv_sec =0;
return;
}

	ns_to_timespec(cur_setting->it_interval, timr->it.mmtimer.incr * sgi_clock_period);
	ns_to_timespec(cur_setting->it_value, (timr->it.mmtimer.expires - rtc_time())* sgi_clock_period);
	return;
}
