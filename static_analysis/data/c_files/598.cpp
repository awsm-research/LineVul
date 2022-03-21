static MagickBooleanType SkipDXTMipmaps(Image *image,DDSInfo *dds_info,
int texel_size,ExceptionInfo *exception)
{
register ssize_t
i;

MagickOffsetType
offset;

size_t
h,
w;

/*
Only skip mipmaps for textures and cube maps
*/
if (EOFBlob(image) != MagickFalse)
{
ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
image->filename);
return(MagickFalse);
}
if (dds_info->ddscaps1 & DDSCAPS_MIPMAP
&& (dds_info->ddscaps1 & DDSCAPS_TEXTURE
|| dds_info->ddscaps2 & DDSCAPS2_CUBEMAP))
{
w = DIV2(dds_info->width);
h = DIV2(dds_info->height);

/*
Mipmapcount includes the main image, so start from one
*/
for (i = 1; (i < (ssize_t) dds_info->mipmapcount) && w && h; i++)
{
offset = (MagickOffsetType) ((w + 3) / 4) * ((h + 3) / 4) * texel_size;
        (void) SeekBlob(image, offset, SEEK_CUR);
w = DIV2(w);
h = DIV2(h);
}
}
return(MagickTrue);
}
