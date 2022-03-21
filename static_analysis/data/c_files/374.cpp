image_transform_png_set_expand_16_add(image_transform *this,
    PNG_CONST image_transform **that, png_byte colour_type, png_byte bit_depth)
{
UNUSED(colour_type)

this->next = *that;
*that = this;

/* expand_16 does something unless the bit depth is already 16. */
return bit_depth < 16;
}
