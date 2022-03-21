tiffcp(TIFF* in, TIFF* out)
{
	uint16 bitspersample, samplesperpixel = 1;
uint16 input_compression, input_photometric = PHOTOMETRIC_MINISBLACK;
copyFunc cf;
uint32 width, length;
struct cpTag* p;

CopyField(TIFFTAG_IMAGEWIDTH, width);
CopyField(TIFFTAG_IMAGELENGTH, length);
CopyField(TIFFTAG_BITSPERSAMPLE, bitspersample);
CopyField(TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
if (compression != (uint16)-1)
TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
else
CopyField(TIFFTAG_COMPRESSION, compression);
TIFFGetFieldDefaulted(in, TIFFTAG_COMPRESSION, &input_compression);
TIFFGetFieldDefaulted(in, TIFFTAG_PHOTOMETRIC, &input_photometric);
if (input_compression == COMPRESSION_JPEG) {
/* Force conversion to RGB */
TIFFSetField(in, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
} else if (input_photometric == PHOTOMETRIC_YCBCR) {
/* Otherwise, can't handle subsampled input */
uint16 subsamplinghor,subsamplingver;

TIFFGetFieldDefaulted(in, TIFFTAG_YCBCRSUBSAMPLING,
&subsamplinghor, &subsamplingver);
if (subsamplinghor!=1 || subsamplingver!=1) {
fprintf(stderr, "tiffcp: %s: Can't copy/convert subsampled image.\n",
TIFFFileName(in));
return FALSE;
}
}
if (compression == COMPRESSION_JPEG) {
if (input_photometric == PHOTOMETRIC_RGB &&
jpegcolormode == JPEGCOLORMODE_RGB)
TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_YCBCR);
else
TIFFSetField(out, TIFFTAG_PHOTOMETRIC, input_photometric);
}
else if (compression == COMPRESSION_SGILOG
|| compression == COMPRESSION_SGILOG24)
TIFFSetField(out, TIFFTAG_PHOTOMETRIC,
samplesperpixel == 1 ?
PHOTOMETRIC_LOGL : PHOTOMETRIC_LOGLUV);
else if (input_compression == COMPRESSION_JPEG &&
samplesperpixel == 3 ) {
/* RGB conversion was forced above
hence the output will be of the same type */
TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
}
else
CopyTag(TIFFTAG_PHOTOMETRIC, 1, TIFF_SHORT);
if (fillorder != 0)
TIFFSetField(out, TIFFTAG_FILLORDER, fillorder);
else
CopyTag(TIFFTAG_FILLORDER, 1, TIFF_SHORT);
/*
* Will copy `Orientation' tag from input image
*/
TIFFGetFieldDefaulted(in, TIFFTAG_ORIENTATION, &orientation);
switch (orientation) {
case ORIENTATION_BOTRIGHT:
case ORIENTATION_RIGHTBOT:	/* XXX */
TIFFWarning(TIFFFileName(in), "using bottom-left orientation");
orientation = ORIENTATION_BOTLEFT;
/* fall thru... */
case ORIENTATION_LEFTBOT:	/* XXX */
case ORIENTATION_BOTLEFT:
break;
case ORIENTATION_TOPRIGHT:
case ORIENTATION_RIGHTTOP:	/* XXX */
default:
TIFFWarning(TIFFFileName(in), "using top-left orientation");
orientation = ORIENTATION_TOPLEFT;
/* fall thru... */
case ORIENTATION_LEFTTOP:	/* XXX */
case ORIENTATION_TOPLEFT:
break;
}
TIFFSetField(out, TIFFTAG_ORIENTATION, orientation);
/*
* Choose tiles/strip for the output image according to
* the command line arguments (-tiles, -strips) and the
* structure of the input image.
*/
if (outtiled == -1)
outtiled = TIFFIsTiled(in);
if (outtiled) {
/*
* Setup output file's tile width&height.  If either
* is not specified, use either the value from the
* input image or, if nothing is defined, use the
* library default.
*/
if (tilewidth == (uint32) -1)
TIFFGetField(in, TIFFTAG_TILEWIDTH, &tilewidth);
if (tilelength == (uint32) -1)
TIFFGetField(in, TIFFTAG_TILELENGTH, &tilelength);
TIFFDefaultTileSize(out, &tilewidth, &tilelength);
TIFFSetField(out, TIFFTAG_TILEWIDTH, tilewidth);
TIFFSetField(out, TIFFTAG_TILELENGTH, tilelength);
} else {
/*
* RowsPerStrip is left unspecified: use either the
* value from the input image or, if nothing is defined,
* use the library default.
*/
if (rowsperstrip == (uint32) 0) {
if (!TIFFGetField(in, TIFFTAG_ROWSPERSTRIP,
&rowsperstrip)) {
rowsperstrip =
TIFFDefaultStripSize(out, rowsperstrip);
}
if (rowsperstrip > length && rowsperstrip != (uint32)-1)
rowsperstrip = length;
}
else if (rowsperstrip == (uint32) -1)
rowsperstrip = length;
TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
}
if (config != (uint16) -1)
TIFFSetField(out, TIFFTAG_PLANARCONFIG, config);
else
CopyField(TIFFTAG_PLANARCONFIG, config);
if (samplesperpixel <= 4)
CopyTag(TIFFTAG_TRANSFERFUNCTION, 4, TIFF_SHORT);
CopyTag(TIFFTAG_COLORMAP, 4, TIFF_SHORT);
/* SMinSampleValue & SMaxSampleValue */
switch (compression) {
case COMPRESSION_JPEG:
TIFFSetField(out, TIFFTAG_JPEGQUALITY, quality);
TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, jpegcolormode);
break;
case COMPRESSION_JBIG:
CopyTag(TIFFTAG_FAXRECVPARAMS, 1, TIFF_LONG);
CopyTag(TIFFTAG_FAXRECVTIME, 1, TIFF_LONG);
CopyTag(TIFFTAG_FAXSUBADDRESS, 1, TIFF_ASCII);
CopyTag(TIFFTAG_FAXDCS, 1, TIFF_ASCII);
break;
case COMPRESSION_LZW:
case COMPRESSION_ADOBE_DEFLATE:
case COMPRESSION_DEFLATE:
case COMPRESSION_LZMA:
if (predictor != (uint16)-1)
TIFFSetField(out, TIFFTAG_PREDICTOR, predictor);
else
CopyField(TIFFTAG_PREDICTOR, predictor);
if (preset != -1) {
if (compression == COMPRESSION_ADOBE_DEFLATE
|| compression == COMPRESSION_DEFLATE)
TIFFSetField(out, TIFFTAG_ZIPQUALITY, preset);
else if (compression == COMPRESSION_LZMA)
TIFFSetField(out, TIFFTAG_LZMAPRESET, preset);
}
break;
case COMPRESSION_CCITTFAX3:
case COMPRESSION_CCITTFAX4:
if (compression == COMPRESSION_CCITTFAX3) {
if (g3opts != (uint32) -1)
TIFFSetField(out, TIFFTAG_GROUP3OPTIONS,
g3opts);
else
CopyField(TIFFTAG_GROUP3OPTIONS, g3opts);
} else
CopyTag(TIFFTAG_GROUP4OPTIONS, 1, TIFF_LONG);
CopyTag(TIFFTAG_BADFAXLINES, 1, TIFF_LONG);
CopyTag(TIFFTAG_CLEANFAXDATA, 1, TIFF_LONG);
CopyTag(TIFFTAG_CONSECUTIVEBADFAXLINES, 1, TIFF_LONG);
CopyTag(TIFFTAG_FAXRECVPARAMS, 1, TIFF_LONG);
CopyTag(TIFFTAG_FAXRECVTIME, 1, TIFF_LONG);
CopyTag(TIFFTAG_FAXSUBADDRESS, 1, TIFF_ASCII);
break;
}
{
uint32 len32;
void** data;
if (TIFFGetField(in, TIFFTAG_ICCPROFILE, &len32, &data))
TIFFSetField(out, TIFFTAG_ICCPROFILE, len32, data);
}
{
uint16 ninks;
const char* inknames;
if (TIFFGetField(in, TIFFTAG_NUMBEROFINKS, &ninks)) {
TIFFSetField(out, TIFFTAG_NUMBEROFINKS, ninks);
if (TIFFGetField(in, TIFFTAG_INKNAMES, &inknames)) {
int inknameslen = strlen(inknames) + 1;
const char* cp = inknames;
while (ninks > 1) {
cp = strchr(cp, '\0');
cp++;
inknameslen += (strlen(cp) + 1);
ninks--;
}
TIFFSetField(out, TIFFTAG_INKNAMES, inknameslen, inknames);
}
}
}
{
unsigned short pg0, pg1;

if (pageInSeq == 1) {
if (pageNum < 0) /* only one input file */ {
if (TIFFGetField(in, TIFFTAG_PAGENUMBER, &pg0, &pg1))
TIFFSetField(out, TIFFTAG_PAGENUMBER, pg0, pg1);
} else
TIFFSetField(out, TIFFTAG_PAGENUMBER, pageNum++, 0);

} else {
if (TIFFGetField(in, TIFFTAG_PAGENUMBER, &pg0, &pg1)) {
if (pageNum < 0) /* only one input file */
TIFFSetField(out, TIFFTAG_PAGENUMBER, pg0, pg1);
else
TIFFSetField(out, TIFFTAG_PAGENUMBER, pageNum++, 0);
}
}
}

for (p = tags; p < &tags[NTAGS]; p++)
CopyTag(p->tag, p->count, p->type);

cf = pickCopyFunc(in, out, bitspersample, samplesperpixel);
return (cf ? (*cf)(in, out, length, width, samplesperpixel) : FALSE);
}
