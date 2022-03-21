image_transform_png_set_scale_16_add(image_transform *this,
    PNG_CONST image_transform **that, png_byte colour_type, png_byte bit_depth)
{
UNUSED(colour_type)

this->next = *that;
*that = this;

return bit_depth > 8;
}
