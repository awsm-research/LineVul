static int tight_fill_palette(VncState *vs, int x, int y,
size_t count, uint32_t *bg, uint32_t *fg,
VncPalette **palette)
{
int max;

max = count / tight_conf[vs->tight.compression].idx_max_colors_divisor;
if (max < 2 &&
count >= tight_conf[vs->tight.compression].mono_min_rect_size) {
max = 2;
}
if (max >= 256) {
max = 256;
}

    switch(vs->clientds.pf.bytes_per_pixel) {
case 4:
return tight_fill_palette32(vs, x, y, max, count, bg, fg, palette);
case 2:
return tight_fill_palette16(vs, x, y, max, count, bg, fg, palette);
default:
max = 2;
return tight_fill_palette8(vs, x, y, max, count, bg, fg, palette);
}
return 0;
}
