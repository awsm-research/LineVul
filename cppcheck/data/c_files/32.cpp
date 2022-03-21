FT_Bitmap_Embolden( FT_Library  library,
FT_Bitmap*  bitmap,
FT_Pos      xStrength,
FT_Pos      yStrength )
{
FT_Error        error;
unsigned char*  p;
    FT_Int          i, x, y, pitch;
FT_Int          xstr, ystr;


return FT_THROW( Invalid_Library_Handle );

if ( !bitmap || !bitmap->buffer )
return FT_THROW( Invalid_Argument );

if ( ( ( FT_PIX_ROUND( xStrength ) >> 6 ) > FT_INT_MAX ) ||
( ( FT_PIX_ROUND( yStrength ) >> 6 ) > FT_INT_MAX ) )
return FT_THROW( Invalid_Argument );

xstr = (FT_Int)FT_PIX_ROUND( xStrength ) >> 6;
ystr = (FT_Int)FT_PIX_ROUND( yStrength ) >> 6;

if ( xstr == 0 && ystr == 0 )
return FT_Err_Ok;
else if ( xstr < 0 || ystr < 0 )
return FT_THROW( Invalid_Argument );

switch ( bitmap->pixel_mode )
{
case FT_PIXEL_MODE_GRAY2:
case FT_PIXEL_MODE_GRAY4:
{
FT_Bitmap  tmp;


/* convert to 8bpp */
FT_Bitmap_New( &tmp );
error = FT_Bitmap_Convert( library, bitmap, &tmp, 1 );
if ( error )
return error;

FT_Bitmap_Done( library, bitmap );
*bitmap = tmp;
}
break;

case FT_PIXEL_MODE_MONO:
if ( xstr > 8 )
xstr = 8;
break;

case FT_PIXEL_MODE_LCD:
xstr *= 3;
break;

case FT_PIXEL_MODE_LCD_V:
ystr *= 3;
break;

case FT_PIXEL_MODE_BGRA:
/* We don't embolden color glyphs. */
return FT_Err_Ok;
}

error = ft_bitmap_assure_buffer( library->memory, bitmap, xstr, ystr );
if ( error )
return error;

/* take care of bitmap flow */
pitch = bitmap->pitch;
if ( pitch > 0 )
p = bitmap->buffer + pitch * ystr;
else
{
pitch = -pitch;
p = bitmap->buffer + pitch * ( bitmap->rows - 1 );
}

/* for each row */
for ( y = 0; y < bitmap->rows ; y++ )
{
/*
* Horizontally:
*
* From the last pixel on, make each pixel or'ed with the
* `xstr' pixels before it.
*/
for ( x = pitch - 1; x >= 0; x-- )
{
unsigned char  tmp;


tmp = p[x];
for ( i = 1; i <= xstr; i++ )
{
if ( bitmap->pixel_mode == FT_PIXEL_MODE_MONO )
{
p[x] |= tmp >> i;

/* the maximum value of 8 for `xstr' comes from here */
if ( x > 0 )
p[x] |= p[x - 1] << ( 8 - i );

#if 0
if ( p[x] == 0xff )
break;
#endif
}
else
{
if ( x - i >= 0 )
{
if ( p[x] + p[x - i] > bitmap->num_grays - 1 )
{
p[x] = (unsigned char)( bitmap->num_grays - 1 );
break;
}
else
{
p[x] = (unsigned char)( p[x] + p[x - i] );
if ( p[x] == bitmap->num_grays - 1 )
break;
}
}
else
break;
}
}
}

/*
* Vertically:
*
* Make the above `ystr' rows or'ed with it.
*/
for ( x = 1; x <= ystr; x++ )
{
unsigned char*  q;


q = p - bitmap->pitch * x;
for ( i = 0; i < pitch; i++ )
q[i] |= p[i];
}

p += bitmap->pitch;
}

bitmap->width += xstr;
bitmap->rows += ystr;

return FT_Err_Ok;
}
