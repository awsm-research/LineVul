void *load_device_tree(const char *filename_path, int *sizep)
{
int dt_size;
int dt_file_load_size;
int ret;
void *fdt = NULL;

*sizep = 0;
dt_size = get_image_size(filename_path);
if (dt_size < 0) {
error_report("Unable to get size of device tree file '%s'",
filename_path);
goto fail;
}

/* Expand to 2x size to give enough room for manipulation.  */
dt_size += 10000;
dt_size *= 2;
/* First allocate space in qemu for device tree */
fdt = g_malloc0(dt_size);

    dt_file_load_size = load_image(filename_path, fdt);
if (dt_file_load_size < 0) {
error_report("Unable to open device tree file '%s'",
filename_path);
goto fail;
}

ret = fdt_open_into(fdt, fdt, dt_size);
if (ret) {
error_report("Unable to copy device tree in memory");
goto fail;
}

/* Check sanity of device tree */
if (fdt_check_header(fdt)) {
error_report("Device tree file loaded into memory is invalid: %s",
filename_path);
goto fail;
}
*sizep = dt_size;
return fdt;

fail:
g_free(fdt);
return NULL;
}
