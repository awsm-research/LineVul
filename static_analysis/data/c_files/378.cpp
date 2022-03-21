get_control(png_const_structrp png_ptr)
{

/* This just returns the (file*).  The chunk and idat control structures
* don't always exist.
*/
   struct control *control = png_voidcast(struct control*,
png_get_error_ptr(png_ptr));
return &control->file;
}
