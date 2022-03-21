vips_foreign_load_gif_scan_image( VipsForeignLoadGif *gif )
{
VipsObjectClass *class = VIPS_OBJECT_GET_CLASS( gif );
GifFileType *file = gif->file;
	ColorMapObject *map = file->Image.ColorMap ?
		file->Image.ColorMap : file->SColorMap;

GifByteType *extension;

if( DGifGetImageDesc( gif->file ) == GIF_ERROR ) {
vips_foreign_load_gif_error( gif );
return( -1 );
}

/* Check that the frame looks sane. Perhaps giflib checks
* this for us.
*/
if( file->Image.Left < 0 ||
file->Image.Width < 1 ||
file->Image.Width > 10000 ||
file->Image.Left + file->Image.Width > file->SWidth ||
file->Image.Top < 0 ||
file->Image.Height < 1 ||
file->Image.Height > 10000 ||
file->Image.Top + file->Image.Height > file->SHeight ) {
vips_error( class->nickname, "%s", _( "bad frame size" ) );
return( -1 );
}

/* Test for a non-greyscale colourmap for this frame.
*/
if( !gif->has_colour &&
map ) {
int i;

for( i = 0; i < map->ColorCount; i++ )
if( map->Colors[i].Red != map->Colors[i].Green ||
map->Colors[i].Green != map->Colors[i].Blue ) {
gif->has_colour = TRUE;
break;
}
}

/* Step over compressed image data.
*/
do {
if( vips_foreign_load_gif_code_next( gif, &extension ) )
return( -1 );
} while( extension != NULL );

return( 0 );
}
