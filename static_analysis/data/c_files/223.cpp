static enum AVPixelFormat h263_get_format(AVCodecContext *avctx)
{
/* MPEG-4 Studio Profile only, not supported by hardware */
if (avctx->bits_per_raw_sample > 8) {
        av_assert1(avctx->profile == FF_PROFILE_MPEG4_SIMPLE_STUDIO);
return avctx->pix_fmt;
}

if (avctx->codec->id == AV_CODEC_ID_MSS2)
return AV_PIX_FMT_YUV420P;

if (CONFIG_GRAY && (avctx->flags & AV_CODEC_FLAG_GRAY)) {
if (avctx->color_range == AVCOL_RANGE_UNSPECIFIED)
avctx->color_range = AVCOL_RANGE_MPEG;
return AV_PIX_FMT_GRAY8;
}

return avctx->pix_fmt = ff_get_format(avctx, avctx->codec->pix_fmts);
}
