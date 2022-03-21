void DecoderTest::RunLoop(CompressedVideoSource *video) {
  vpx_codec_dec_cfg_t dec_cfg = {0};
  Decoder* const decoder = codec_->CreateDecoder(dec_cfg, 0);
ASSERT_TRUE(decoder != NULL);

// Decode frames.
  for (video->Begin(); video->cxdata(); video->Next()) {
PreDecodeFrameHook(*video, decoder);
    vpx_codec_err_t res_dec = decoder->DecodeFrame(video->cxdata(),
                                                   video->frame_size());
    ASSERT_EQ(VPX_CODEC_OK, res_dec) << decoder->DecodeError();

DxDataIterator dec_iter = decoder->GetDxData();
const vpx_image_t *img = NULL;

// Get decompressed data

while ((img = dec_iter.Next()))
DecompressedFrameHook(*img, video->frame_number());
}
delete decoder;
}
