static ssize_t WritePSDChannels(const PSDInfo *psd_info,
const ImageInfo *image_info,Image *image,Image *next_image,
MagickOffsetType size_offset,const MagickBooleanType separate)
{
Image
*mask;

MagickOffsetType
rows_offset;

size_t
channels,
count,
length,
offset_length;

unsigned char
*compact_pixels;

count=0;
offset_length=0;
rows_offset=0;
compact_pixels=(unsigned char *) NULL;
if (next_image->compression == RLECompression)
{
      compact_pixels=AcquireCompactPixels(image);
if (compact_pixels == (unsigned char *) NULL)
return(0);
}
channels=1;
if (separate == MagickFalse)
{
if (next_image->storage_class != PseudoClass)
{
if (IsGrayImage(next_image,&next_image->exception) == MagickFalse)
channels=next_image->colorspace == CMYKColorspace ? 4 : 3;
if (next_image->matte != MagickFalse)
channels++;
}
rows_offset=TellBlob(image)+2;
count+=WriteCompressionStart(psd_info,image,next_image,channels);
offset_length=(next_image->rows*(psd_info->version == 1 ? 2 : 4));
}
size_offset+=2;
if (next_image->storage_class == PseudoClass)
{
length=WritePSDChannel(psd_info,image_info,image,next_image,
IndexQuantum,compact_pixels,rows_offset,separate);
if (separate != MagickFalse)
size_offset+=WritePSDSize(psd_info,image,length,size_offset)+2;
else
rows_offset+=offset_length;
count+=length;
}
else
{
if (IsGrayImage(next_image,&next_image->exception) != MagickFalse)
{
length=WritePSDChannel(psd_info,image_info,image,next_image,
GrayQuantum,compact_pixels,rows_offset,separate);
if (separate != MagickFalse)
size_offset+=WritePSDSize(psd_info,image,length,size_offset)+2;
else
rows_offset+=offset_length;
count+=length;
}
else
{
if (next_image->colorspace == CMYKColorspace)
(void) NegateImage(next_image,MagickFalse);

length=WritePSDChannel(psd_info,image_info,image,next_image,
RedQuantum,compact_pixels,rows_offset,separate);
if (separate != MagickFalse)
size_offset+=WritePSDSize(psd_info,image,length,size_offset)+2;
else
rows_offset+=offset_length;
count+=length;

length=WritePSDChannel(psd_info,image_info,image,next_image,
GreenQuantum,compact_pixels,rows_offset,separate);
if (separate != MagickFalse)
size_offset+=WritePSDSize(psd_info,image,length,size_offset)+2;
else
rows_offset+=offset_length;
count+=length;

length=WritePSDChannel(psd_info,image_info,image,next_image,
BlueQuantum,compact_pixels,rows_offset,separate);
if (separate != MagickFalse)
size_offset+=WritePSDSize(psd_info,image,length,size_offset)+2;
else
rows_offset+=offset_length;
count+=length;

if (next_image->colorspace == CMYKColorspace)
{
length=WritePSDChannel(psd_info,image_info,image,next_image,
BlackQuantum,compact_pixels,rows_offset,separate);
if (separate != MagickFalse)
size_offset+=WritePSDSize(psd_info,image,length,size_offset)+2;
else
rows_offset+=offset_length;
count+=length;
}
}
if (next_image->matte != MagickFalse)
{
length=WritePSDChannel(psd_info,image_info,image,next_image,
AlphaQuantum,compact_pixels,rows_offset,separate);
if (separate != MagickFalse)
size_offset+=WritePSDSize(psd_info,image,length,size_offset)+2;
else
rows_offset+=offset_length;
count+=length;
}
}
compact_pixels=(unsigned char *) RelinquishMagickMemory(compact_pixels);
if (next_image->colorspace == CMYKColorspace)
(void) NegateImage(next_image,MagickFalse);
if (separate != MagickFalse)
{
const char
*property;

property=GetImageArtifact(next_image,"psd:opacity-mask");
if (property != (const char *) NULL)
{
mask=(Image *) GetImageRegistry(ImageRegistryType,property,
&image->exception);
if (mask != (Image *) NULL)
{
if (mask->compression == RLECompression)
{
compact_pixels=AcquireCompactPixels(mask);
if (compact_pixels == (unsigned char *) NULL)
return(0);
}
length=WritePSDChannel(psd_info,image_info,image,mask,
RedQuantum,compact_pixels,rows_offset,MagickTrue);
(void) WritePSDSize(psd_info,image,length,size_offset);
count+=length;
compact_pixels=(unsigned char *) RelinquishMagickMemory(
compact_pixels);
}
}
}
return(count);
}
