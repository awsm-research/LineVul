void RunScrollbarThumbDragLatencyTest() {
    // See above comment in RunScrollbarButtonLatencyTest for why this test
    // doesn't run on Android.
#if !defined(OS_ANDROID)
    // Click on the scrollbar thumb and drag it twice to induce a compositor
    // thread scrollbar ScrollBegin and ScrollUpdate.
    blink::WebFloatPoint scrollbar_thumb(795, 30);
    blink::WebMouseEvent mouse_down = SyntheticWebMouseEventBuilder::Build(
        blink::WebInputEvent::kMouseDown, scrollbar_thumb.x, scrollbar_thumb.y,
        0);
    mouse_down.button = blink::WebMouseEvent::Button::kLeft;
    mouse_down.SetTimeStamp(base::TimeTicks::Now());
    GetWidgetHost()->ForwardMouseEvent(mouse_down);
    blink::WebMouseEvent mouse_move = SyntheticWebMouseEventBuilder::Build(
        blink::WebInputEvent::kMouseMove, scrollbar_thumb.x,
        scrollbar_thumb.y + 10, 0);
    mouse_move.button = blink::WebMouseEvent::Button::kLeft;
    mouse_move.SetTimeStamp(base::TimeTicks::Now());
    GetWidgetHost()->ForwardMouseEvent(mouse_move);
    RunUntilInputProcessed(GetWidgetHost());
    mouse_move.SetPositionInWidget(scrollbar_thumb.x, scrollbar_thumb.y + 20);
    mouse_move.SetPositionInScreen(scrollbar_thumb.x, scrollbar_thumb.y + 20);
    GetWidgetHost()->ForwardMouseEvent(mouse_move);
    RunUntilInputProcessed(GetWidgetHost());
    blink::WebMouseEvent mouse_up = SyntheticWebMouseEventBuilder::Build(
        blink::WebInputEvent::kMouseUp, scrollbar_thumb.x,
        scrollbar_thumb.y + 20, 0);
    mouse_up.button = blink::WebMouseEvent::Button::kLeft;
    mouse_up.SetTimeStamp(base::TimeTicks::Now());
    GetWidgetHost()->ForwardMouseEvent(mouse_up);
    RunUntilInputProcessed(GetWidgetHost());
    FetchHistogramsFromChildProcesses();
    const std::string scroll_types[] = {"ScrollBegin", "ScrollUpdate"};
    for (const std::string& scroll_type : scroll_types) {
      EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
          1, "Event.Latency." + scroll_type +
                 ".Scrollbar.TimeToScrollUpdateSwapBegin4"));
      EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
          1, "Event.Latency." + scroll_type +
                 ".Scrollbar.RendererSwapToBrowserNotified2"));
      EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
          1, "Event.Latency." + scroll_type +
                 ".Scrollbar.BrowserNotifiedToBeforeGpuSwap2"));
      EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
          1, "Event.Latency." + scroll_type + ".Scrollbar.GpuSwap2"));
      std::string thread_name =
          DoesScrollbarScrollOnMainThread() ? "Main" : "Impl";
      EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
          1, "Event.Latency." + scroll_type + ".Scrollbar.TimeToHandled2_" +
                 thread_name));
      EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
          1, "Event.Latency." + scroll_type +
                 ".Scrollbar.HandledToRendererSwap2_" + thread_name));
    }
#endif  // !defined(OS_ANDROID)
  }
