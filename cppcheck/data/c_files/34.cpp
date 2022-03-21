standard_row_validate(standard_display *dp, png_const_structp pp,
int iImage, int iDisplay, png_uint_32 y)
{
int where;
png_byte std[STANDARD_ROWMAX];

/* The row must be pre-initialized to the magic number here for the size
* tests to pass:
*/
memset(std, 178, sizeof std);
standard_row(pp, std, dp->id, y);

/* At the end both the 'row' and 'display' arrays should end up identical.

* In earlier passes 'row' will be partially filled in, with only the pixels
* that have been read so far, but 'display' will have those pixels
* replicated to fill the unread pixels while reading an interlaced image.
#if PNG_LIBPNG_VER < 10506
    * The side effect inside the libpng sequential reader is that the 'row'
    * array retains the correct values for unwritten pixels within the row
    * bytes, while the 'display' array gets bits off the end of the image (in
    * the last byte) trashed.  Unfortunately in the progressive reader the
    * row bytes are always trashed, so we always do a pixel_cmp here even though
    * a memcmp of all cbRow bytes will succeed for the sequential reader.
#endif
*/
if (iImage >= 0 &&
(where = pixel_cmp(std, store_image_row(dp->ps, pp, iImage, y),
dp->bit_width)) != 0)
{
char msg[64];
sprintf(msg, "PNG image row[%lu][%d] changed from %.2x to %.2x",
(unsigned long)y, where-1, std[where-1],
store_image_row(dp->ps, pp, iImage, y)[where-1]);

png_error(pp, msg);
}

#if PNG_LIBPNG_VER < 10506
   /* In this case use pixel_cmp because we need to compare a partial
    * byte at the end of the row if the row is not an exact multiple
    * of 8 bits wide.  (This is fixed in libpng-1.5.6 and pixel_cmp is
    * changed to match!)
    *
#endif
if (iDisplay >= 0 &&
(where = pixel_cmp(std, store_image_row(dp->ps, pp, iDisplay, y),
dp->bit_width)) != 0)
{
char msg[64];
      sprintf(msg, "display  row[%lu][%d] changed from %.2x to %.2x",
(unsigned long)y, where-1, std[where-1],
store_image_row(dp->ps, pp, iDisplay, y)[where-1]);
png_error(pp, msg);
}
}
