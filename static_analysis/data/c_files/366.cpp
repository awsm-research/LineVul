static int decode_nal_unit(HEVCContext *s, const H2645NAL *nal)
{
HEVCLocalContext *lc = s->HEVClc;
GetBitContext *gb    = &lc->gb;
int ctb_addr_ts, ret;

*gb              = nal->gb;
s->nal_unit_type = nal->type;
s->temporal_id   = nal->temporal_id;

switch (s->nal_unit_type) {
case HEVC_NAL_VPS:
if (s->avctx->hwaccel && s->avctx->hwaccel->decode_params) {
ret = s->avctx->hwaccel->decode_params(s->avctx,
nal->type,
nal->raw_data,
nal->raw_size);
if (ret < 0)
goto fail;
}
ret = ff_hevc_decode_nal_vps(gb, s->avctx, &s->ps);
if (ret < 0)
goto fail;
break;
case HEVC_NAL_SPS:
if (s->avctx->hwaccel && s->avctx->hwaccel->decode_params) {
ret = s->avctx->hwaccel->decode_params(s->avctx,
nal->type,
nal->raw_data,
nal->raw_size);
if (ret < 0)
goto fail;
}
ret = ff_hevc_decode_nal_sps(gb, s->avctx, &s->ps,
s->apply_defdispwin);
if (ret < 0)
goto fail;
break;
case HEVC_NAL_PPS:
if (s->avctx->hwaccel && s->avctx->hwaccel->decode_params) {
ret = s->avctx->hwaccel->decode_params(s->avctx,
nal->type,
nal->raw_data,
nal->raw_size);
if (ret < 0)
goto fail;
}
ret = ff_hevc_decode_nal_pps(gb, s->avctx, &s->ps);
if (ret < 0)
goto fail;
break;
case HEVC_NAL_SEI_PREFIX:
case HEVC_NAL_SEI_SUFFIX:
if (s->avctx->hwaccel && s->avctx->hwaccel->decode_params) {
ret = s->avctx->hwaccel->decode_params(s->avctx,
nal->type,
nal->raw_data,
nal->raw_size);
if (ret < 0)
goto fail;
}
ret = ff_hevc_decode_nal_sei(gb, s->avctx, &s->sei, &s->ps, s->nal_unit_type);
if (ret < 0)
goto fail;
break;
case HEVC_NAL_TRAIL_R:
case HEVC_NAL_TRAIL_N:
case HEVC_NAL_TSA_N:
case HEVC_NAL_TSA_R:
case HEVC_NAL_STSA_N:
case HEVC_NAL_STSA_R:
case HEVC_NAL_BLA_W_LP:
case HEVC_NAL_BLA_W_RADL:
case HEVC_NAL_BLA_N_LP:
case HEVC_NAL_IDR_W_RADL:
case HEVC_NAL_IDR_N_LP:
case HEVC_NAL_CRA_NUT:
case HEVC_NAL_RADL_N:
case HEVC_NAL_RADL_R:
case HEVC_NAL_RASL_N:
case HEVC_NAL_RASL_R:
ret = hls_slice_header(s);
if (ret < 0)
return ret;

if (
(s->avctx->skip_frame >= AVDISCARD_BIDIR && s->sh.slice_type == HEVC_SLICE_B) ||
(s->avctx->skip_frame >= AVDISCARD_NONINTRA && s->sh.slice_type != HEVC_SLICE_I) ||
(s->avctx->skip_frame >= AVDISCARD_NONKEY && !IS_IRAP(s))) {
break;
}

if (s->sh.first_slice_in_pic_flag) {
            if (s->ref) {
                av_log(s->avctx, AV_LOG_ERROR, "Two slices reporting being the first in the same frame.\n");
                goto fail;
            }
if (s->max_ra == INT_MAX) {
if (s->nal_unit_type == HEVC_NAL_CRA_NUT || IS_BLA(s)) {
s->max_ra = s->poc;
} else {
if (IS_IDR(s))
s->max_ra = INT_MIN;
}
}

if ((s->nal_unit_type == HEVC_NAL_RASL_R || s->nal_unit_type == HEVC_NAL_RASL_N) &&
s->poc <= s->max_ra) {
s->is_decoded = 0;
break;
} else {
if (s->nal_unit_type == HEVC_NAL_RASL_R && s->poc > s->max_ra)
s->max_ra = INT_MIN;
}

s->overlap ++;
ret = hevc_frame_start(s);
if (ret < 0)
return ret;
} else if (!s->ref) {
av_log(s->avctx, AV_LOG_ERROR, "First slice in a frame missing.\n");
goto fail;
}

if (s->nal_unit_type != s->first_nal_type) {
av_log(s->avctx, AV_LOG_ERROR,
"Non-matching NAL types of the VCL NALUs: %d %d\n",
s->first_nal_type, s->nal_unit_type);
return AVERROR_INVALIDDATA;
}

if (!s->sh.dependent_slice_segment_flag &&
s->sh.slice_type != HEVC_SLICE_I) {
ret = ff_hevc_slice_rpl(s);
if (ret < 0) {
av_log(s->avctx, AV_LOG_WARNING,
"Error constructing the reference lists for the current slice.\n");
goto fail;
}
}

if (s->sh.first_slice_in_pic_flag && s->avctx->hwaccel) {
ret = s->avctx->hwaccel->start_frame(s->avctx, NULL, 0);
if (ret < 0)
goto fail;
}

if (s->avctx->hwaccel) {
ret = s->avctx->hwaccel->decode_slice(s->avctx, nal->raw_data, nal->raw_size);
if (ret < 0)
goto fail;
} else {
if (s->threads_number > 1 && s->sh.num_entry_point_offsets > 0)
ctb_addr_ts = hls_slice_data_wpp(s, nal);
else
ctb_addr_ts = hls_slice_data(s);
if (ctb_addr_ts >= (s->ps.sps->ctb_width * s->ps.sps->ctb_height)) {
s->is_decoded = 1;
}

if (ctb_addr_ts < 0) {
ret = ctb_addr_ts;
goto fail;
}
}
break;
case HEVC_NAL_EOS_NUT:
case HEVC_NAL_EOB_NUT:
s->seq_decode = (s->seq_decode + 1) & 0xff;
s->max_ra     = INT_MAX;
break;
case HEVC_NAL_AUD:
case HEVC_NAL_FD_NUT:
break;
default:
av_log(s->avctx, AV_LOG_INFO,
"Skipping NAL unit %d\n", s->nal_unit_type);
}

return 0;
fail:
if (s->avctx->err_recognition & AV_EF_EXPLODE)
return ret;
return 0;
}
