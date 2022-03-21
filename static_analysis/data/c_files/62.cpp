void Initialize(ChannelLayout channel_layout, int bits_per_channel) {
AudioParameters params(
media::AudioParameters::AUDIO_PCM_LINEAR, channel_layout,
        kSamplesPerSecond, bits_per_channel, kRawDataSize);

algorithm_.Initialize(1, params, base::Bind(
&AudioRendererAlgorithmTest::EnqueueData, base::Unretained(this)));
EnqueueData();
}
