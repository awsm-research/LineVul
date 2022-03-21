png_write_destroy(png_structp png_ptr)
{
#ifdef PNG_SETJMP_SUPPORTED
jmp_buf tmp_jmp; /* Save jump buffer */
#endif
png_error_ptr error_fn;
png_error_ptr warning_fn;
png_voidp error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
png_free_ptr free_fn;
#endif

png_debug(1, "in png_write_destroy");

/* Free any memory zlib uses */
deflateEnd(&png_ptr->zstream);

/* Free our memory.  png_free checks NULL for us. */
png_free(png_ptr, png_ptr->zbuf);
png_free(png_ptr, png_ptr->row_buf);
#ifdef PNG_WRITE_FILTER_SUPPORTED
png_free(png_ptr, png_ptr->prev_row);
png_free(png_ptr, png_ptr->sub_row);
png_free(png_ptr, png_ptr->up_row);
png_free(png_ptr, png_ptr->avg_row);
png_free(png_ptr, png_ptr->paeth_row);
#endif

#ifdef PNG_TIME_RFC1123_SUPPORTED
png_free(png_ptr, png_ptr->time_buffer);
#endif

#ifdef PNG_WRITE_WEIGHTED_FILTER_SUPPORTED
   png_free(png_ptr, png_ptr->prev_filters);
   png_free(png_ptr, png_ptr->filter_weights);
   png_free(png_ptr, png_ptr->inv_filter_weights);
   png_free(png_ptr, png_ptr->filter_costs);
   png_free(png_ptr, png_ptr->inv_filter_costs);
#endif
#ifdef PNG_SETJMP_SUPPORTED
/* Reset structure */
png_memcpy(tmp_jmp, png_ptr->jmpbuf, png_sizeof(jmp_buf));
#endif

error_fn = png_ptr->error_fn;
warning_fn = png_ptr->warning_fn;
error_ptr = png_ptr->error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
free_fn = png_ptr->free_fn;
#endif

png_memset(png_ptr, 0, png_sizeof(png_struct));

png_ptr->error_fn = error_fn;
png_ptr->warning_fn = warning_fn;
png_ptr->error_ptr = error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
png_ptr->free_fn = free_fn;
#endif

#ifdef PNG_SETJMP_SUPPORTED
png_memcpy(png_ptr->jmpbuf, tmp_jmp, png_sizeof(jmp_buf));
#endif
}
