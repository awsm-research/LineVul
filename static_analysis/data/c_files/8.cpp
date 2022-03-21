static MagickBooleanType ReadDXT5(Image *image, DDSInfo *dds_info,
ExceptionInfo *exception)
{
DDSColors
colors;

ssize_t
j,
y;

MagickSizeType
alpha_bits;

PixelPacket
*q;

register ssize_t
i,
x;

unsigned char
a0,
a1;

size_t
alpha,
bits,
code,
alpha_code;

unsigned short
c0,
c1;

for (y = 0; y < (ssize_t) dds_info->height; y += 4)
{
for (x = 0; x < (ssize_t) dds_info->width; x += 4)
{
/* Get 4x4 patch of pixels to write on */
      q = QueueAuthenticPixels(image, x, y, Min(4, dds_info->width - x),
                         Min(4, dds_info->height - y),exception);

if (q == (PixelPacket *) NULL)
return MagickFalse;

/* Read alpha values (8 bytes) */
a0 = (unsigned char) ReadBlobByte(image);
a1 = (unsigned char) ReadBlobByte(image);

alpha_bits = (MagickSizeType)ReadBlobLSBLong(image);
alpha_bits = alpha_bits | ((MagickSizeType)ReadBlobLSBShort(image) << 32);

/* Read 8 bytes of data from the image */
c0 = ReadBlobLSBShort(image);
c1 = ReadBlobLSBShort(image);
bits = ReadBlobLSBLong(image);

CalculateColors(c0, c1, &colors, MagickTrue);

/* Write the pixels */
for (j = 0; j < 4; j++)
{
for (i = 0; i < 4; i++)
{
if ((x + i) < (ssize_t) dds_info->width && (y + j) < (ssize_t) dds_info->height)
{
code = (bits >> ((4*j+i)*2)) & 0x3;
SetPixelRed(q,ScaleCharToQuantum(colors.r[code]));
SetPixelGreen(q,ScaleCharToQuantum(colors.g[code]));
SetPixelBlue(q,ScaleCharToQuantum(colors.b[code]));
/* Extract alpha value */
alpha_code = (size_t) (alpha_bits >> (3*(4*j+i))) & 0x7;
if (alpha_code == 0)
alpha = a0;
else if (alpha_code == 1)
alpha = a1;
else if (a0 > a1)
alpha = ((8-alpha_code) * a0 + (alpha_code-1) * a1) / 7;
else if (alpha_code == 6)
alpha = 0;
else if (alpha_code == 7)
alpha = 255;
else
alpha = (((6-alpha_code) * a0 + (alpha_code-1) * a1) / 5);
SetPixelAlpha(q,ScaleCharToQuantum((unsigned char)
alpha));
q++;
}
}
}

if (SyncAuthenticPixels(image,exception) == MagickFalse)
return MagickFalse;
}
}

  SkipDXTMipmaps(image, dds_info, 16);
  return MagickTrue;
}
