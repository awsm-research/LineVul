write_png(const char **name, FILE *fp, int color_type, int bit_depth,
volatile png_fixed_point gamma, chunk_insert * volatile insert,
   unsigned int filters, unsigned int *colors)
{
png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
name, makepng_error, makepng_warning);
volatile png_infop info_ptr = NULL;
volatile png_bytep row = NULL;

if (png_ptr == NULL)
{
fprintf(stderr, "makepng: OOM allocating write structure\n");
return 1;
}

if (setjmp(png_jmpbuf(png_ptr)))
{
png_structp nv_ptr = png_ptr;
png_infop nv_info = info_ptr;

png_ptr = NULL;
info_ptr = NULL;
png_destroy_write_struct(&nv_ptr, &nv_info);
if (row != NULL) free(row);
return 1;
}


/* Allow benign errors so that we can write PNGs with errors */
png_set_benign_errors(png_ptr, 1/*allowed*/);
png_init_io(png_ptr, fp);

info_ptr = png_create_info_struct(png_ptr);
if (info_ptr == NULL)

png_error(png_ptr, "OOM allocating info structure");

{
      unsigned int size = image_size_of_type(color_type, bit_depth, colors);
png_fixed_point real_gamma = 45455; /* For sRGB */
png_byte gamma_table[256];
double conv;

/* This function uses the libpng values used on read to carry extra
* information about the gamma:
*/
if (gamma == PNG_GAMMA_MAC_18)
gamma = 65909;

else if (gamma > 0 && gamma < 1000)
gamma = PNG_FP_1;

if (gamma > 0)
real_gamma = gamma;

{
unsigned int i;

if (real_gamma == 45455) for (i=0; i<256; ++i)
{
gamma_table[i] = (png_byte)i;
conv = 1.;
}

else
{
/* Convert 'i' from sRGB (45455) to real_gamma, this makes
* the images look the same regardless of the gAMA chunk.
*/
conv = real_gamma;
conv /= 45455;


gamma_table[0] = 0;

for (i=1; i<255; ++i)
               gamma_table[i] = (png_byte)floor(pow(i/255.,conv) * 255 + .5);

gamma_table[255] = 255;
}
}

      png_set_IHDR(png_ptr, info_ptr, size, size, bit_depth, color_type,
PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

if (color_type & PNG_COLOR_MASK_PALETTE)
{
int npalette;
png_color palette[256];
png_byte trans[256];


npalette = generate_palette(palette, trans, bit_depth, gamma_table,
colors);
png_set_PLTE(png_ptr, info_ptr, palette, npalette);
         png_set_tRNS(png_ptr, info_ptr, trans, npalette-1,
            NULL/*transparent color*/);

/* Reset gamma_table to prevent the image rows being changed */
for (npalette=0; npalette<256; ++npalette)
gamma_table[npalette] = (png_byte)npalette;
}

if (gamma == PNG_DEFAULT_sRGB)
png_set_sRGB(png_ptr, info_ptr, PNG_sRGB_INTENT_ABSOLUTE);

else if (gamma > 0) /* Else don't set color space information */
{
png_set_gAMA_fixed(png_ptr, info_ptr, real_gamma);

/* Just use the sRGB values here. */
png_set_cHRM_fixed(png_ptr, info_ptr,
/* color      x       y */
/* white */ 31270, 32900,
/* red   */ 64000, 33000,
/* green */ 30000, 60000,
/* blue  */ 15000, 6000
);
}

/* Insert extra information. */
while (insert != NULL)
{
insert->insert(png_ptr, info_ptr, insert->nparams, insert->parameters);
insert = insert->next;
}

/* Write the file header. */
png_write_info(png_ptr, info_ptr);

/* Restrict the filters */

png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, filters);

{
         int passes = png_set_interlace_handling(png_ptr);
int pass;
png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

row = malloc(rowbytes);

if (row == NULL)
png_error(png_ptr, "OOM allocating row buffer");

for (pass = 0; pass < passes; ++pass)

{
unsigned int y;

            for (y=0; y<size; ++y)
{
               generate_row(row, rowbytes, y, color_type, bit_depth,
                  gamma_table, conv, colors);
png_write_row(png_ptr, row);
}
}
}
}

/* Finish writing the file. */
png_write_end(png_ptr, info_ptr);

{
png_structp nv_ptr = png_ptr;
png_infop nv_info = info_ptr;

png_ptr = NULL;
info_ptr = NULL;
png_destroy_write_struct(&nv_ptr, &nv_info);
}
free(row);
return 0;
}
