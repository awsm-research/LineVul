int read_header_tga(gdIOCtx *ctx, oTga *tga)
{

unsigned char header[18];

if (gdGetBuf(header, sizeof(header), ctx) < 18) {
gd_error("fail to read header");
return -1;
}

tga->identsize = header[0];
tga->colormaptype = header[1];
tga->imagetype = header[2];
tga->colormapstart = header[3] + (header[4] << 8);
tga->colormaplength = header[5] + (header[6] << 8);
tga->colormapbits = header[7];
tga->xstart = header[8] + (header[9] << 8);
tga->ystart = header[10] + (header[11] << 8);
tga->width = header[12] + (header[13] << 8);
tga->height = header[14] + (header[15] << 8);
tga->bits = header[16];
tga->alphabits = header[17] & 0x0f;
tga->fliph = (header[17] & 0x10) ? 1 : 0;
tga->flipv = (header[17] & 0x20) ? 0 : 1;

#if DEBUG
printf("format bps: %i\n", tga->bits);
printf("flip h/v: %i / %i\n", tga->fliph, tga->flipv);
printf("alpha: %i\n", tga->alphabits);
printf("wxh: %i %i\n", tga->width, tga->height);
#endif

	switch(tga->bits) {
	case 8:
	case 16:
	case 24:
	case 32:
		break;
	default:
		gd_error("bps %i not supported", tga->bits);
return -1;
		break;
}

tga->ident = NULL;

if (tga->identsize > 0) {
tga->ident = (char *) gdMalloc(tga->identsize * sizeof(char));
if(tga->ident == NULL) {
return -1;
}

gdGetBuf(tga->ident, tga->identsize, ctx);
}

return 1;
}
