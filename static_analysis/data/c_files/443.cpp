void TestPlaybackRate(double playback_rate) {
    static const int kDefaultBufferSize = kSamplesPerSecond / 10;
    static const int kDefaultFramesRequested = 5 * kSamplesPerSecond;

TestPlaybackRate(playback_rate, kDefaultBufferSize,
kDefaultFramesRequested);
}
