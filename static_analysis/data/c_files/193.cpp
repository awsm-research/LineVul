jas_image_t *bmp_decode(jas_stream_t *in, char *optstr)
{
jas_image_t *image;
bmp_hdr_t hdr;
bmp_info_t *info;
uint_fast16_t cmptno;
jas_image_cmptparm_t cmptparms[3];
jas_image_cmptparm_t *cmptparm;
uint_fast16_t numcmpts;
long n;

if (optstr) {
jas_eprintf("warning: ignoring BMP decoder options\n");
}

jas_eprintf(
"THE BMP FORMAT IS NOT FULLY SUPPORTED!\n"
"THAT IS, THE JASPER SOFTWARE CANNOT DECODE ALL TYPES OF BMP DATA.\n"
"IF YOU HAVE ANY PROBLEMS, PLEASE TRY CONVERTING YOUR IMAGE DATA\n"
"TO THE PNM FORMAT, AND USING THIS FORMAT INSTEAD.\n"
);

/* Read the bitmap header. */
if (bmp_gethdr(in, &hdr)) {
jas_eprintf("cannot get header\n");
		return 0;
}
JAS_DBGLOG(1, (
"BMP header: magic 0x%x; siz %d; res1 %d; res2 %d; off %d\n",
hdr.magic, hdr.siz, hdr.reserved1, hdr.reserved2, hdr.off
));

/* Read the bitmap information. */
if (!(info = bmp_getinfo(in))) {
jas_eprintf("cannot get info\n");
		return 0;
}
JAS_DBGLOG(1,
	  ("BMP information: len %d; width %d; height %d; numplanes %d; "
	  "depth %d; enctype %d; siz %d; hres %d; vres %d; numcolors %d; "
	  "mincolors %d\n", info->len, info->width, info->height, info->numplanes,
	  info->depth, info->enctype, info->siz, info->hres, info->vres,
	  info->numcolors, info->mincolors));

/* Ensure that we support this type of BMP file. */
if (!bmp_issupported(&hdr, info)) {
jas_eprintf("error: unsupported BMP encoding\n");
		bmp_info_destroy(info);
		return 0;
}

/* Skip over any useless data between the end of the palette
and start of the bitmap data. */
if ((n = hdr.off - (BMP_HDRLEN + BMP_INFOLEN + BMP_PALLEN(info))) < 0) {
jas_eprintf("error: possibly bad bitmap offset?\n");
		return 0;
}
if (n > 0) {
jas_eprintf("skipping unknown data in BMP file\n");
if (bmp_gobble(in, n)) {
			bmp_info_destroy(info);
			return 0;
}
}

/* Get the number of components. */
numcmpts = bmp_numcmpts(info);

for (cmptno = 0, cmptparm = cmptparms; cmptno < numcmpts; ++cmptno,
++cmptparm) {
cmptparm->tlx = 0;
cmptparm->tly = 0;
cmptparm->hstep = 1;
cmptparm->vstep = 1;
cmptparm->width = info->width;
cmptparm->height = info->height;
cmptparm->prec = 8;
cmptparm->sgnd = false;
}

/* Create image object. */
if (!(image = jas_image_create(numcmpts, cmptparms,
JAS_CLRSPC_UNKNOWN))) {
		bmp_info_destroy(info);
		return 0;
}

if (numcmpts == 3) {
jas_image_setclrspc(image, JAS_CLRSPC_SRGB);
jas_image_setcmpttype(image, 0,
JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
jas_image_setcmpttype(image, 1,
JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
jas_image_setcmpttype(image, 2,
JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
} else {
jas_image_setclrspc(image, JAS_CLRSPC_SGRAY);
jas_image_setcmpttype(image, 0,
JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
}

/* Read the bitmap data. */
if (bmp_getdata(in, info, image)) {
		bmp_info_destroy(info);
		jas_image_destroy(image);
		return 0;
}

bmp_info_destroy(info);

return image;
}
