freeimage(Image *image)
{
freebuffer(image);
png_image_free(&image->image);

if (image->input_file != NULL)
{
fclose(image->input_file);
image->input_file = NULL;
}

if (image->input_memory != NULL)
{
free(image->input_memory);
image->input_memory = NULL;
image->input_memory_size = 0;
}


if (image->tmpfile_name[0] != 0 && (image->opts & KEEP_TMPFILES) == 0)
{
      remove(image->tmpfile_name);
image->tmpfile_name[0] = 0;
}
}
