NetworkThrottleManagerImpl::NetworkThrottleManagerImpl()
: lifetime_median_estimate_(PercentileEstimator::kMedianPercentile,
kInitialMedianInMs),
outstanding_recomputation_timer_(
          base::MakeUnique<base::Timer>(false /* retain_user_task */,
false /* is_repeating */)),
tick_clock_(new base::DefaultTickClock()),
weak_ptr_factory_(this) {}
