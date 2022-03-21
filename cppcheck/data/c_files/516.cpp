static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
AVFilterContext *ctx = inlink->dst;
LutContext *s = ctx->priv;
AVFilterLink *outlink = ctx->outputs[0];
AVFrame *out;
uint8_t *inrow, *outrow, *inrow0, *outrow0;
int i, j, plane, direct = 0;

if (av_frame_is_writable(in)) {
direct = 1;
out = in;
} else {
out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
if (!out) {
av_frame_free(&in);
return AVERROR(ENOMEM);
}
av_frame_copy_props(out, in);
}

if (s->is_rgb) {
/* packed */
inrow0  = in ->data[0];
outrow0 = out->data[0];

for (i = 0; i < in->height; i ++) {
int w = inlink->w;
const uint8_t (*tab)[256] = (const uint8_t (*)[256])s->lut;
inrow  = inrow0;
outrow = outrow0;
for (j = 0; j < w; j++) {
switch (s->step) {
case 4:  outrow[3] = tab[3][inrow[3]]; // Fall-through
case 3:  outrow[2] = tab[2][inrow[2]]; // Fall-through
case 2:  outrow[1] = tab[1][inrow[1]]; // Fall-through
default: outrow[0] = tab[0][inrow[0]];
}
outrow += s->step;
inrow  += s->step;
}
inrow0  += in ->linesize[0];
outrow0 += out->linesize[0];
}
} else {
/* planar */
        for (plane = 0; plane < 4 && in->data[plane]; plane++) {
int vsub = plane == 1 || plane == 2 ? s->vsub : 0;
int hsub = plane == 1 || plane == 2 ? s->hsub : 0;
int h = FF_CEIL_RSHIFT(inlink->h, vsub);
int w = FF_CEIL_RSHIFT(inlink->w, hsub);

inrow  = in ->data[plane];
outrow = out->data[plane];

for (i = 0; i < h; i++) {
const uint8_t *tab = s->lut[plane];
for (j = 0; j < w; j++)
outrow[j] = tab[inrow[j]];
inrow  += in ->linesize[plane];
outrow += out->linesize[plane];
}
}
}

if (!direct)
av_frame_free(&in);

return ff_filter_frame(outlink, out);
}
