void MediaElementAudioSourceHandler::Process(size_t number_of_frames) {
AudioBus* output_bus = Output(0).Bus();

// Use a tryLock() to avoid contention in the real-time audio thread.
// If we fail to acquire the lock then the HTMLMediaElement must be in the
// middle of reconfiguring its playback engine, so we output silence in this
// case.
MutexTryLocker try_locker(process_lock_);
if (try_locker.Locked()) {
if (!MediaElement() || !source_sample_rate_) {
output_bus->Zero();
return;
}

// TODO(crbug.com/811516): Although OnSetFormat() requested the output bus
// channels, the actual channel count might have not been changed yet.
// Output silence for such case until the channel count is resolved.
if (source_number_of_channels_ != output_bus->NumberOfChannels()) {
output_bus->Zero();
return;
}

AudioSourceProvider& provider = MediaElement()->GetAudioSourceProvider();
// Grab data from the provider so that the element continues to make
// progress, even if we're going to output silence anyway.
if (multi_channel_resampler_.get()) {
DCHECK_NE(source_sample_rate_, Context()->sampleRate());
multi_channel_resampler_->Process(&provider, output_bus,
number_of_frames);
} else {
// Bypass the resampler completely if the source is at the context's
// sample-rate.
DCHECK_EQ(source_sample_rate_, Context()->sampleRate());
provider.ProvideInput(output_bus, number_of_frames);
}
// Output silence if we don't have access to the element.
    if (!PassesCORSAccessCheck()) {
      if (maybe_print_cors_message_) {
        // Print a CORS message, but just once for each change in the current
        // media element source, and only if we have a document to print to.
        maybe_print_cors_message_ = false;
        PostCrossThreadTask(
            *task_runner_, FROM_HERE,
            CrossThreadBind(&MediaElementAudioSourceHandler::PrintCORSMessage,
                            WrapRefCounted(this), current_src_string_));
      }
output_bus->Zero();
}
} else {
// We failed to acquire the lock.
output_bus->Zero();
}
}
