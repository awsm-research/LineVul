static int zrle_send_framebuffer_update(VncState *vs, int x, int y,
int w, int h)
{
    bool be = !!(vs->clientds.flags & QEMU_BIG_ENDIAN_FLAG);
size_t bytes;
int zywrle_level;

if (vs->zrle.type == VNC_ENCODING_ZYWRLE) {
if (!vs->vd->lossy || vs->tight.quality == (uint8_t)-1
|| vs->tight.quality == 9) {
zywrle_level = 0;
vs->zrle.type = VNC_ENCODING_ZRLE;
} else if (vs->tight.quality < 3) {
zywrle_level = 3;
} else if (vs->tight.quality < 6) {
zywrle_level = 2;
} else {
zywrle_level = 1;
}
} else {
zywrle_level = 0;
}

vnc_zrle_start(vs);

    switch(vs->clientds.pf.bytes_per_pixel) {
case 1:
zrle_encode_8ne(vs, x, y, w, h, zywrle_level);
break;

case 2:
        if (vs->clientds.pf.gmax > 0x1F) {
if (be) {
zrle_encode_16be(vs, x, y, w, h, zywrle_level);
} else {
zrle_encode_16le(vs, x, y, w, h, zywrle_level);
}
} else {
if (be) {
zrle_encode_15be(vs, x, y, w, h, zywrle_level);
} else {
zrle_encode_15le(vs, x, y, w, h, zywrle_level);
}
}
break;

case 4:
{
bool fits_in_ls3bytes;
bool fits_in_ms3bytes;

fits_in_ls3bytes =
            ((vs->clientds.pf.rmax << vs->clientds.pf.rshift) < (1 << 24) &&
             (vs->clientds.pf.gmax << vs->clientds.pf.gshift) < (1 << 24) &&
             (vs->clientds.pf.bmax << vs->clientds.pf.bshift) < (1 << 24));

        fits_in_ms3bytes = (vs->clientds.pf.rshift > 7 &&
                            vs->clientds.pf.gshift > 7 &&
                            vs->clientds.pf.bshift > 7);

if ((fits_in_ls3bytes && !be) || (fits_in_ms3bytes && be)) {
if (be) {
zrle_encode_24abe(vs, x, y, w, h, zywrle_level);
} else {
zrle_encode_24ale(vs, x, y, w, h, zywrle_level);
}
} else if ((fits_in_ls3bytes && be) || (fits_in_ms3bytes && !be)) {
if (be) {
zrle_encode_24bbe(vs, x, y, w, h, zywrle_level);
} else {
zrle_encode_24ble(vs, x, y, w, h, zywrle_level);
}
} else {
if (be) {
zrle_encode_32be(vs, x, y, w, h, zywrle_level);
} else {
zrle_encode_32le(vs, x, y, w, h, zywrle_level);
}
}
}
break;
}

vnc_zrle_stop(vs);
bytes = zrle_compress_data(vs, Z_DEFAULT_COMPRESSION);
vnc_framebuffer_update(vs, x, y, w, h, vs->zrle.type);
vnc_write_u32(vs, bytes);
vnc_write(vs, vs->zrle.zlib.buffer, vs->zrle.zlib.offset);
return 1;
}
