static int mem_resize(jas_stream_memobj_t *m, int bufsize)
{
unsigned char *buf;

//assert(m->buf_);
	assert(bufsize >= 0);

	JAS_DBGLOG(100, ("mem_resize(%p, %d)\n", m, bufsize));
if (!(buf = jas_realloc2(m->buf_, bufsize, sizeof(unsigned char))) &&
bufsize) {
JAS_DBGLOG(100, ("mem_resize realloc failed\n"));
return -1;
}
JAS_DBGLOG(100, ("mem_resize realloc succeeded\n"));
m->buf_ = buf;
m->bufsize_ = bufsize;
return 0;
}
