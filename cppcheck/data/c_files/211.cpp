loadImage(TIFF* in, struct image_data *image, struct dump_opts *dump, unsigned char **read_ptr)
{
uint32   i;
float    xres = 0.0, yres = 0.0;
  uint16   nstrips = 0, ntiles = 0, planar = 0;
uint16   bps = 0, spp = 0, res_unit = 0;
uint16   orientation = 0;
uint16   input_compression = 0, input_photometric = 0;
uint16   subsampling_horiz, subsampling_vert;
uint32   width = 0, length = 0;
uint32   stsize = 0, tlsize = 0, buffsize = 0, scanlinesize = 0;
uint32   tw = 0, tl = 0;       /* Tile width and length */
uint32   tile_rowsize = 0;
unsigned char *read_buff = NULL;
unsigned char *new_buff  = NULL;
int      readunit = 0;
static   uint32  prev_readsize = 0;

TIFFGetFieldDefaulted(in, TIFFTAG_BITSPERSAMPLE, &bps);
TIFFGetFieldDefaulted(in, TIFFTAG_SAMPLESPERPIXEL, &spp);
TIFFGetFieldDefaulted(in, TIFFTAG_PLANARCONFIG, &planar);
TIFFGetFieldDefaulted(in, TIFFTAG_ORIENTATION, &orientation);
if (! TIFFGetFieldDefaulted(in, TIFFTAG_PHOTOMETRIC, &input_photometric))
TIFFError("loadImage","Image lacks Photometric interpreation tag");
if (! TIFFGetField(in, TIFFTAG_IMAGEWIDTH,  &width))
TIFFError("loadimage","Image lacks image width tag");
if(! TIFFGetField(in, TIFFTAG_IMAGELENGTH, &length))
TIFFError("loadimage","Image lacks image length tag");
TIFFGetFieldDefaulted(in, TIFFTAG_XRESOLUTION, &xres);
TIFFGetFieldDefaulted(in, TIFFTAG_YRESOLUTION, &yres);
if (!TIFFGetFieldDefaulted(in, TIFFTAG_RESOLUTIONUNIT, &res_unit))
res_unit = RESUNIT_INCH;
if (!TIFFGetField(in, TIFFTAG_COMPRESSION, &input_compression))
input_compression = COMPRESSION_NONE;

#ifdef DEBUG2
char compressionid[16];

switch (input_compression)
{
case COMPRESSION_NONE:	/* 1  dump mode */
strcpy (compressionid, "None/dump");
break;
case COMPRESSION_CCITTRLE:	  /* 2 CCITT modified Huffman RLE */
strcpy (compressionid, "Huffman RLE");
break;
case COMPRESSION_CCITTFAX3:	  /* 3 CCITT Group 3 fax encoding */
strcpy (compressionid, "Group3 Fax");
break;
case COMPRESSION_CCITTFAX4:	  /* 4 CCITT Group 4 fax encoding */
strcpy (compressionid, "Group4 Fax");
break;
case COMPRESSION_LZW:	  /* 5 Lempel-Ziv  & Welch */
strcpy (compressionid, "LZW");
break;
case COMPRESSION_OJPEG:	  /* 6 !6.0 JPEG */
strcpy (compressionid, "Old Jpeg");
break;
case COMPRESSION_JPEG:	  /* 7 %JPEG DCT compression */
strcpy (compressionid, "New Jpeg");
break;
case COMPRESSION_NEXT:	  /* 32766 NeXT 2-bit RLE */
strcpy (compressionid, "Next RLE");
break;
case COMPRESSION_CCITTRLEW:   /* 32771 #1 w/ word alignment */
strcpy (compressionid, "CITTRLEW");
break;
case COMPRESSION_PACKBITS:	  /* 32773 Macintosh RLE */
strcpy (compressionid, "Mac Packbits");
break;
case COMPRESSION_THUNDERSCAN: /* 32809 ThunderScan RLE */
strcpy (compressionid, "Thunderscan");
break;
case COMPRESSION_IT8CTPAD:	  /* 32895 IT8 CT w/padding */
strcpy (compressionid, "IT8 padded");
break;
case COMPRESSION_IT8LW:	  /* 32896 IT8 Linework RLE */
strcpy (compressionid, "IT8 RLE");
break;
case COMPRESSION_IT8MP:	  /* 32897 IT8 Monochrome picture */
strcpy (compressionid, "IT8 mono");
break;
case COMPRESSION_IT8BL:	  /* 32898 IT8 Binary line art */
strcpy (compressionid, "IT8 lineart");
break;
case COMPRESSION_PIXARFILM:	  /* 32908 Pixar companded 10bit LZW */
strcpy (compressionid, "Pixar 10 bit");
break;
case COMPRESSION_PIXARLOG:	  /* 32909 Pixar companded 11bit ZIP */
strcpy (compressionid, "Pixar 11bit");
break;
case COMPRESSION_DEFLATE:	  /* 32946 Deflate compression */
strcpy (compressionid, "Deflate");
break;
case COMPRESSION_ADOBE_DEFLATE: /* 8 Deflate compression */
strcpy (compressionid, "Adobe deflate");
break;
default:
strcpy (compressionid, "None/unknown");
break;
}
TIFFError("loadImage", "Input compression %s", compressionid);
#endif

scanlinesize = TIFFScanlineSize(in);
image->bps = bps;
image->spp = spp;
image->planar = planar;
image->width = width;
image->length = length;
image->xres = xres;
image->yres = yres;
image->res_unit = res_unit;
image->compression = input_compression;
image->photometric = input_photometric;
#ifdef DEBUG2
char photometricid[12];

switch (input_photometric)
{
case PHOTOMETRIC_MINISWHITE:
strcpy (photometricid, "MinIsWhite");
break;
case PHOTOMETRIC_MINISBLACK:
strcpy (photometricid, "MinIsBlack");
break;
case PHOTOMETRIC_RGB:
strcpy (photometricid, "RGB");
break;
case PHOTOMETRIC_PALETTE:
strcpy (photometricid, "Palette");
break;
case PHOTOMETRIC_MASK:
strcpy (photometricid, "Mask");
break;
case PHOTOMETRIC_SEPARATED:
strcpy (photometricid, "Separated");
break;
case PHOTOMETRIC_YCBCR:
strcpy (photometricid, "YCBCR");
break;
case PHOTOMETRIC_CIELAB:
strcpy (photometricid, "CIELab");
break;
case PHOTOMETRIC_ICCLAB:
strcpy (photometricid, "ICCLab");
break;
case PHOTOMETRIC_ITULAB:
strcpy (photometricid, "ITULab");
break;
case PHOTOMETRIC_LOGL:
strcpy (photometricid, "LogL");
break;
case PHOTOMETRIC_LOGLUV:
strcpy (photometricid, "LOGLuv");
break;
default:
strcpy (photometricid, "Unknown");
break;
}
TIFFError("loadImage", "Input photometric interpretation %s", photometricid);

#endif
image->orientation = orientation;
switch (orientation)
{
case 0:
case ORIENTATION_TOPLEFT:
image->adjustments = 0;
break;
case ORIENTATION_TOPRIGHT:
image->adjustments = MIRROR_HORIZ;
break;
case ORIENTATION_BOTRIGHT:
image->adjustments = ROTATECW_180;
break;
case ORIENTATION_BOTLEFT:
image->adjustments = MIRROR_VERT;
break;
case ORIENTATION_LEFTTOP:
image->adjustments = MIRROR_VERT | ROTATECW_90;
break;
case ORIENTATION_RIGHTTOP:
image->adjustments = ROTATECW_90;
break;
case ORIENTATION_RIGHTBOT:
image->adjustments = MIRROR_VERT | ROTATECW_270;
break;
case ORIENTATION_LEFTBOT:
image->adjustments = ROTATECW_270;
break;
default:
image->adjustments = 0;
image->orientation = ORIENTATION_TOPLEFT;
}

if ((bps == 0) || (spp == 0))
{
TIFFError("loadImage", "Invalid samples per pixel (%d) or bits per sample (%d)",
spp, bps);
return (-1);
}

if (TIFFIsTiled(in))
{
readunit = TILE;
tlsize = TIFFTileSize(in);
ntiles = TIFFNumberOfTiles(in);
TIFFGetField(in, TIFFTAG_TILEWIDTH, &tw);
TIFFGetField(in, TIFFTAG_TILELENGTH, &tl);

tile_rowsize  = TIFFTileRowSize(in);
if (ntiles == 0 || tlsize == 0 || tile_rowsize == 0)
{
TIFFError("loadImage", "File appears to be tiled, but the number of tiles, tile size, or tile rowsize is zero.");
exit(-1);
}
buffsize = tlsize * ntiles;
if (tlsize != (buffsize / ntiles))
{
TIFFError("loadImage", "Integer overflow when calculating buffer size");
exit(-1);
}

if (buffsize < (uint32)(ntiles * tl * tile_rowsize))
{
buffsize = ntiles * tl * tile_rowsize;
if (ntiles != (buffsize / tl / tile_rowsize))
{
TIFFError("loadImage", "Integer overflow when calculating buffer size");
exit(-1);
}

#ifdef DEBUG2
TIFFError("loadImage",
"Tilesize %u is too small, using ntiles * tilelength * tilerowsize %lu",
tlsize, (unsigned long)buffsize);
#endif
}

if (dump->infile != NULL)
dump_info (dump->infile, dump->format, "",
"Tilesize: %u, Number of Tiles: %u, Tile row size: %u",
tlsize, ntiles, tile_rowsize);
}
else
{
uint32 buffsize_check;
readunit = STRIP;
TIFFGetFieldDefaulted(in, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
stsize = TIFFStripSize(in);
nstrips = TIFFNumberOfStrips(in);
if (nstrips == 0 || stsize == 0)
{
TIFFError("loadImage", "File appears to be striped, but the number of stipes or stripe size is zero.");
exit(-1);
}

buffsize = stsize * nstrips;
if (stsize != (buffsize / nstrips))
{
TIFFError("loadImage", "Integer overflow when calculating buffer size");
exit(-1);
}
buffsize_check = ((length * width * spp * bps) + 7);
if (length != ((buffsize_check - 7) / width / spp / bps))
{
TIFFError("loadImage", "Integer overflow detected.");
exit(-1);
}
if (buffsize < (uint32) (((length * width * spp * bps) + 7) / 8))
{
buffsize =  ((length * width * spp * bps) + 7) / 8;
#ifdef DEBUG2
TIFFError("loadImage",
"Stripsize %u is too small, using imagelength * width * spp * bps / 8 = %lu",
stsize, (unsigned long)buffsize);
#endif
}

if (dump->infile != NULL)
dump_info (dump->infile, dump->format, "",
"Stripsize: %u, Number of Strips: %u, Rows per Strip: %u, Scanline size: %u",
stsize, nstrips, rowsperstrip, scanlinesize);
}

if (input_compression == COMPRESSION_JPEG)
{  /* Force conversion to RGB */
jpegcolormode = JPEGCOLORMODE_RGB;
TIFFSetField(in, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
}
/* The clause up to the read statement is taken from Tom Lane's tiffcp patch */
else
{   /* Otherwise, can't handle subsampled input */
if (input_photometric == PHOTOMETRIC_YCBCR)
{
TIFFGetFieldDefaulted(in, TIFFTAG_YCBCRSUBSAMPLING,
&subsampling_horiz, &subsampling_vert);
if (subsampling_horiz != 1 || subsampling_vert != 1)
{
TIFFError("loadImage",
"Can't copy/convert subsampled image with subsampling %d horiz %d vert",
subsampling_horiz, subsampling_vert);
return (-1);
}
}
}

read_buff = *read_ptr;
/* +3 : add a few guard bytes since reverseSamples16bits() can read a bit */
/* outside buffer */
if (!read_buff)
read_buff = (unsigned char *)_TIFFmalloc(buffsize+3);
else
{
if (prev_readsize < buffsize)
{
new_buff = _TIFFrealloc(read_buff, buffsize+3);
if (!new_buff)
{
free (read_buff);
read_buff = (unsigned char *)_TIFFmalloc(buffsize+3);
}
else
read_buff = new_buff;
}
}
if (!read_buff)
{
TIFFError("loadImage", "Unable to allocate/reallocate read buffer");
return (-1);
}

read_buff[buffsize] = 0;
read_buff[buffsize+1] = 0;
read_buff[buffsize+2] = 0;

prev_readsize = buffsize;
*read_ptr = read_buff;

/* N.B. The read functions used copy separate plane data into a buffer as interleaved
* samples rather than separate planes so the same logic works to extract regions
* regardless of the way the data are organized in the input file.
*/
switch (readunit) {
case STRIP:
if (planar == PLANARCONFIG_CONTIG)
{
if (!(readContigStripsIntoBuffer(in, read_buff)))
{
TIFFError("loadImage", "Unable to read contiguous strips into buffer");
return (-1);
}
}
else
{
if (!(readSeparateStripsIntoBuffer(in, read_buff, length, width, spp, dump)))
{
TIFFError("loadImage", "Unable to read separate strips into buffer");
return (-1);
}
}
break;

case TILE:
if (planar == PLANARCONFIG_CONTIG)
{
if (!(readContigTilesIntoBuffer(in, read_buff, length, width, tw, tl, spp, bps)))
{
TIFFError("loadImage", "Unable to read contiguous tiles into buffer");
return (-1);
}
}
else
{
if (!(readSeparateTilesIntoBuffer(in, read_buff, length, width, tw, tl, spp, bps)))
{
TIFFError("loadImage", "Unable to read separate tiles into buffer");
return (-1);
}
}
break;
default: TIFFError("loadImage", "Unsupported image file format");
return (-1);
break;
}
if ((dump->infile != NULL) && (dump->level == 2))
{
dump_info  (dump->infile, dump->format, "loadImage",
"Image width %d, length %d, Raw image data, %4d bytes",
width, length,  buffsize);
dump_info  (dump->infile, dump->format, "",
"Bits per sample %d, Samples per pixel %d", bps, spp);

for (i = 0; i < length; i++)
dump_buffer(dump->infile, dump->format, 1, scanlinesize,
i, read_buff + (i * scanlinesize));
}
return (0);
}   /* end loadImage */
