image_transform_png_set_expand_add(image_transform *this,
    PNG_CONST image_transform **that, png_byte colour_type, png_byte bit_depth)
{
UNUSED(bit_depth)

this->next = *that;
*that = this;

/* 'expand' should do nothing for RGBA or GA input - no tRNS and the bit
* depth is at least 8 already.
*/
return (colour_type & PNG_COLOR_MASK_ALPHA) == 0;
}
