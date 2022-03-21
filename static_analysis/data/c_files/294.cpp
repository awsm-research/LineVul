static int sgi_clock_set(clockid_t clockid, struct timespec *tp)
{

u64 nsec;
	u64 rem;

nsec = rtc_time() * sgi_clock_period;

	sgi_clock_offset.tv_sec = tp->tv_sec - div_long_long_rem(nsec, NSEC_PER_SEC, &rem);

if (rem <= tp->tv_nsec)
sgi_clock_offset.tv_nsec = tp->tv_sec - rem;
else {
sgi_clock_offset.tv_nsec = tp->tv_sec + NSEC_PER_SEC - rem;
sgi_clock_offset.tv_sec--;
}
return 0;
}
