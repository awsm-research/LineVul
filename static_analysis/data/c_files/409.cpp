virtual void SetUp() {
video_ = new libvpx_test::WebMVideoSource(kVP9TestFile);
ASSERT_TRUE(video_ != NULL);

video_->Init();
video_->Begin();

    vpx_codec_dec_cfg_t cfg = {0};
decoder_ = new libvpx_test::VP9Decoder(cfg, 0);
ASSERT_TRUE(decoder_ != NULL);
}
