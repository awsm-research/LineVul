png_inflate(png_structp png_ptr, const png_byte *data, png_size_t size,
png_bytep output, png_size_t output_size)
{
png_size_t count = 0;

png_ptr->zstream.next_in = (png_bytep)data; /* const_cast: VALID */
png_ptr->zstream.avail_in = size;

while (1)
{
int ret, avail;

/* Reset the output buffer each time round - we empty it
* after every inflate call.
*/
png_ptr->zstream.next_out = png_ptr->zbuf;
png_ptr->zstream.avail_out = png_ptr->zbuf_size;

ret = inflate(&png_ptr->zstream, Z_NO_FLUSH);
avail = png_ptr->zbuf_size - png_ptr->zstream.avail_out;

/* First copy/count any new output - but only if we didn't
* get an error code.
*/
if ((ret == Z_OK || ret == Z_STREAM_END) && avail > 0)
{
if (output != 0 && output_size > count)
{
            int copy = output_size - count;
            if (avail < copy) copy = avail;
png_memcpy(output + count, png_ptr->zbuf, copy);
}
count += avail;
}

if (ret == Z_OK)
continue;

/* Termination conditions - always reset the zstream, it
* must be left in inflateInit state.
*/
png_ptr->zstream.avail_in = 0;
inflateReset(&png_ptr->zstream);

if (ret == Z_STREAM_END)
return count; /* NOTE: may be zero. */

/* Now handle the error codes - the API always returns 0
* and the error message is dumped into the uncompressed
* buffer if available.
*/
{
PNG_CONST char *msg;
if (png_ptr->zstream.msg != 0)
msg = png_ptr->zstream.msg;
else
{
#if defined(PNG_STDIO_SUPPORTED) && !defined(_WIN32_WCE)
char umsg[52];

switch (ret)
{
case Z_BUF_ERROR:
msg = "Buffer error in compressed datastream in %s chunk";
break;
case Z_DATA_ERROR:
msg = "Data error in compressed datastream in %s chunk";
break;
default:
msg = "Incomplete compressed datastream in %s chunk";
break;
}

png_snprintf(umsg, sizeof umsg, msg, png_ptr->chunk_name);
msg = umsg;
#else
msg = "Damaged compressed datastream in chunk other than IDAT";
#endif
}

png_warning(png_ptr, msg);
}

/* 0 means an error - notice that this code simple ignores
* zero length compressed chunks as a result.
*/
return 0;
}
}
