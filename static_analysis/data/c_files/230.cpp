image_transform_png_set_background_set(PNG_CONST image_transform *this,
transform_display *that, png_structp pp, png_infop pi)
{
png_byte colour_type, bit_depth;
png_byte random_bytes[8]; /* 8 bytes - 64 bits - the biggest pixel */
int expand;
png_color_16 back;

/* We need a background colour, because we don't know exactly what transforms
* have been set we have to supply the colour in the original file format and
* so we need to know what that is!  The background colour is stored in the
* transform_display.
*/
RANDOMIZE(random_bytes);

/* Read the random value, for colour type 3 the background colour is actually
* expressed as a 24bit rgb, not an index.
*/
colour_type = that->this.colour_type;
if (colour_type == 3)
{
colour_type = PNG_COLOR_TYPE_RGB;
bit_depth = 8;
expand = 0; /* passing in an RGB not a pixel index */
}


else
{
bit_depth = that->this.bit_depth;
expand = 1;
}

image_pixel_init(&data, random_bytes, colour_type,
      bit_depth, 0/*x*/, 0/*unused: palette*/);

/* Extract the background colour from this image_pixel, but make sure the
* unused fields of 'back' are garbage.
*/
RANDOMIZE(back);

if (colour_type & PNG_COLOR_MASK_COLOR)
{
back.red = (png_uint_16)data.red;
back.green = (png_uint_16)data.green;
back.blue = (png_uint_16)data.blue;
}

else
back.gray = (png_uint_16)data.red;

#  ifdef PNG_FLOATING_POINT_SUPPORTED
png_set_background(pp, &back, PNG_BACKGROUND_GAMMA_FILE, expand, 0);
#  else
png_set_background_fixed(pp, &back, PNG_BACKGROUND_GAMMA_FILE, expand, 0);
#  endif

this->next->set(this->next, that, pp, pi);

}
