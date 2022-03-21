void UkmPageLoadMetricsObserver::RecordTimingMetrics(
const page_load_metrics::mojom::PageLoadTiming& timing,
const page_load_metrics::PageLoadExtraInfo& info) {
ukm::builders::PageLoad builder(info.source_id);
  bool is_user_initiated_navigation =
      // All browser initiated page loads are user-initiated.
      info.user_initiated_info.browser_initiated ||
      // Renderer-initiated navigations are user-initiated if there is an
      // associated input timestamp.
      timing.input_to_navigation_start;
  builder.SetExperimental_Navigation_UserInitiated(
      is_user_initiated_navigation);
if (timing.input_to_navigation_start) {
builder.SetExperimental_InputToNavigationStart(
timing.input_to_navigation_start.value().InMilliseconds());
}
if (timing.parse_timing->parse_start) {
builder.SetParseTiming_NavigationToParseStart(
timing.parse_timing->parse_start.value().InMilliseconds());
}
if (timing.document_timing->dom_content_loaded_event_start) {
builder.SetDocumentTiming_NavigationToDOMContentLoadedEventFired(
timing.document_timing->dom_content_loaded_event_start.value()
.InMilliseconds());
}
if (timing.document_timing->load_event_start) {
builder.SetDocumentTiming_NavigationToLoadEventFired(
timing.document_timing->load_event_start.value().InMilliseconds());
}
if (timing.paint_timing->first_paint) {
builder.SetPaintTiming_NavigationToFirstPaint(
timing.paint_timing->first_paint.value().InMilliseconds());
}
if (timing.paint_timing->first_contentful_paint) {
builder.SetPaintTiming_NavigationToFirstContentfulPaint(
timing.paint_timing->first_contentful_paint.value().InMilliseconds());
}
if (timing.paint_timing->first_meaningful_paint) {
builder.SetExperimental_PaintTiming_NavigationToFirstMeaningfulPaint(
timing.paint_timing->first_meaningful_paint.value().InMilliseconds());
}
if (timing.paint_timing->largest_image_paint.has_value() &&
WasStartedInForegroundOptionalEventInForeground(
timing.paint_timing->largest_image_paint, info)) {
builder.SetExperimental_PaintTiming_NavigationToLargestImagePaint(
timing.paint_timing->largest_image_paint.value().InMilliseconds());
}
if (timing.paint_timing->last_image_paint.has_value() &&
WasStartedInForegroundOptionalEventInForeground(
timing.paint_timing->last_image_paint, info)) {
builder.SetExperimental_PaintTiming_NavigationToLastImagePaint(
timing.paint_timing->last_image_paint.value().InMilliseconds());
}
if (timing.paint_timing->largest_text_paint.has_value() &&
WasStartedInForegroundOptionalEventInForeground(
timing.paint_timing->largest_text_paint, info)) {
builder.SetExperimental_PaintTiming_NavigationToLargestTextPaint(
timing.paint_timing->largest_text_paint.value().InMilliseconds());
}
if (timing.paint_timing->last_text_paint.has_value() &&
WasStartedInForegroundOptionalEventInForeground(
timing.paint_timing->last_text_paint, info)) {
builder.SetExperimental_PaintTiming_NavigationToLastTextPaint(
timing.paint_timing->last_text_paint.value().InMilliseconds());
}
base::Optional<base::TimeDelta> largest_content_paint_time;
uint64_t largest_content_paint_size;
AssignTimeAndSizeForLargestContentfulPaint(largest_content_paint_time,
largest_content_paint_size,
timing.paint_timing);
if (largest_content_paint_size > 0 &&
WasStartedInForegroundOptionalEventInForeground(
largest_content_paint_time, info)) {
builder.SetExperimental_PaintTiming_NavigationToLargestContentPaint(
largest_content_paint_time.value().InMilliseconds());
}
if (timing.interactive_timing->interactive) {
base::TimeDelta time_to_interactive =
timing.interactive_timing->interactive.value();
if (!timing.interactive_timing->first_invalidating_input ||
timing.interactive_timing->first_invalidating_input.value() >
time_to_interactive) {
builder.SetExperimental_NavigationToInteractive(
time_to_interactive.InMilliseconds());
}
}
if (timing.interactive_timing->first_input_delay) {
base::TimeDelta first_input_delay =
timing.interactive_timing->first_input_delay.value();
builder.SetInteractiveTiming_FirstInputDelay2(
first_input_delay.InMilliseconds());
}
if (timing.interactive_timing->first_input_timestamp) {
base::TimeDelta first_input_timestamp =
timing.interactive_timing->first_input_timestamp.value();
builder.SetInteractiveTiming_FirstInputTimestamp2(
first_input_timestamp.InMilliseconds());
}

if (timing.interactive_timing->longest_input_delay) {
base::TimeDelta longest_input_delay =
timing.interactive_timing->longest_input_delay.value();
builder.SetInteractiveTiming_LongestInputDelay2(
longest_input_delay.InMilliseconds());
}
if (timing.interactive_timing->longest_input_timestamp) {
base::TimeDelta longest_input_timestamp =
timing.interactive_timing->longest_input_timestamp.value();
builder.SetInteractiveTiming_LongestInputTimestamp2(
longest_input_timestamp.InMilliseconds());
}

// Use a bucket spacing factor of 1.3 for bytes.
builder.SetNet_CacheBytes(ukm::GetExponentialBucketMin(cache_bytes_, 1.3));
builder.SetNet_NetworkBytes(
ukm::GetExponentialBucketMin(network_bytes_, 1.3));

if (main_frame_timing_)
ReportMainResourceTimingMetrics(timing, &builder);

builder.Record(ukm::UkmRecorder::Get());
}
