Load_SBit_Png( FT_GlyphSlot     slot,
FT_Int           x_offset,
FT_Int           y_offset,
FT_Int           pix_bits,
TT_SBit_Metrics  metrics,
FT_Memory        memory,
FT_Byte*         data,
FT_UInt          png_len,
FT_Bool          populate_map_and_metrics )
{
FT_Bitmap    *map   = &slot->bitmap;
FT_Error      error = FT_Err_Ok;
FT_StreamRec  stream;

png_structp  png;
png_infop    info;
png_uint_32  imgWidth, imgHeight;

int         bitdepth, color_type, interlace;
FT_Int      i;
png_byte*  *rows = NULL; /* pacify compiler */


if ( x_offset < 0 ||
y_offset < 0 )
{
error = FT_THROW( Invalid_Argument );
goto Exit;
}

    if ( !populate_map_and_metrics                   &&
         ( x_offset + metrics->width  > map->width ||
           y_offset + metrics->height > map->rows  ||
           pix_bits != 32                          ||
           map->pixel_mode != FT_PIXEL_MODE_BGRA   ) )
{
error = FT_THROW( Invalid_Argument );
goto Exit;
}

FT_Stream_OpenMemory( &stream, data, png_len );

png = png_create_read_struct( PNG_LIBPNG_VER_STRING,
&error,
error_callback,
warning_callback );
if ( !png )
{
error = FT_THROW( Out_Of_Memory );
goto Exit;
}

info = png_create_info_struct( png );
if ( !info )
{
error = FT_THROW( Out_Of_Memory );
png_destroy_read_struct( &png, NULL, NULL );
goto Exit;
}

if ( ft_setjmp( png_jmpbuf( png ) ) )
{
error = FT_THROW( Invalid_File_Format );
goto DestroyExit;
}

png_set_read_fn( png, &stream, read_data_from_FT_Stream );

png_read_info( png, info );
png_get_IHDR( png, info,
&imgWidth, &imgHeight,
&bitdepth, &color_type, &interlace,
NULL, NULL );

if ( error                                        ||
( !populate_map_and_metrics                &&
( (FT_Int)imgWidth  != metrics->width  ||
(FT_Int)imgHeight != metrics->height ) ) )
goto DestroyExit;

if ( populate_map_and_metrics )
{
FT_Long  size;


metrics->width  = (FT_Int)imgWidth;
metrics->height = (FT_Int)imgHeight;

map->width      = metrics->width;
map->rows       = metrics->height;
map->pixel_mode = FT_PIXEL_MODE_BGRA;
map->pitch      = map->width * 4;
map->num_grays  = 256;

/* reject too large bitmaps similarly to the rasterizer */
if ( map->rows > 0x7FFF || map->width > 0x7FFF )
{
error = FT_THROW( Array_Too_Large );
goto DestroyExit;
}

size = map->rows * map->pitch;

error = ft_glyphslot_alloc_bitmap( slot, size );
if ( error )
goto DestroyExit;
}

/* convert palette/gray image to rgb */
if ( color_type == PNG_COLOR_TYPE_PALETTE )
png_set_palette_to_rgb( png );

/* expand gray bit depth if needed */
if ( color_type == PNG_COLOR_TYPE_GRAY )
{
#if PNG_LIBPNG_VER >= 10209
png_set_expand_gray_1_2_4_to_8( png );
#else
png_set_gray_1_2_4_to_8( png );
#endif
}

/* transform transparency to alpha */
if ( png_get_valid(png, info, PNG_INFO_tRNS ) )
png_set_tRNS_to_alpha( png );

if ( bitdepth == 16 )
png_set_strip_16( png );

if ( bitdepth < 8 )
png_set_packing( png );

/* convert grayscale to RGB */
if ( color_type == PNG_COLOR_TYPE_GRAY       ||
color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
png_set_gray_to_rgb( png );

if ( interlace != PNG_INTERLACE_NONE )
png_set_interlace_handling( png );

png_set_filler( png, 0xFF, PNG_FILLER_AFTER );

/* recheck header after setting EXPAND options */
png_read_update_info(png, info );
png_get_IHDR( png, info,
&imgWidth, &imgHeight,
&bitdepth, &color_type, &interlace,
NULL, NULL );

if ( bitdepth != 8                              ||
!( color_type == PNG_COLOR_TYPE_RGB       ||
color_type == PNG_COLOR_TYPE_RGB_ALPHA ) )
{
error = FT_THROW( Invalid_File_Format );
goto DestroyExit;
}

switch ( color_type )
{
default:
/* Shouldn't happen, but fall through. */

case PNG_COLOR_TYPE_RGB_ALPHA:
png_set_read_user_transform_fn( png, premultiply_data );
break;

case PNG_COLOR_TYPE_RGB:
/* Humm, this smells.  Carry on though. */
png_set_read_user_transform_fn( png, convert_bytes_to_data );
break;
}

if ( FT_NEW_ARRAY( rows, imgHeight ) )
{
error = FT_THROW( Out_Of_Memory );
goto DestroyExit;
}

for ( i = 0; i < (FT_Int)imgHeight; i++ )
rows[i] = map->buffer + ( y_offset + i ) * map->pitch + x_offset * 4;

png_read_image( png, rows );

FT_FREE( rows );

png_read_end( png, info );

DestroyExit:
png_destroy_read_struct( &png, &info, NULL );
FT_Stream_Close( &stream );

Exit:
return error;
}
