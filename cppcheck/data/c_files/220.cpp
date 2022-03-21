jp2_box_t *jp2_box_get(jas_stream_t *in)
{
jp2_box_t *box;
jp2_boxinfo_t *boxinfo;
jas_stream_t *tmpstream;
uint_fast32_t len;
uint_fast64_t extlen;
bool dataflag;

box = 0;
tmpstream = 0;

if (!(box = jas_malloc(sizeof(jp2_box_t)))) {
goto error;
}
box->ops = &jp2_boxinfo_unk.ops;
if (jp2_getuint32(in, &len) || jp2_getuint32(in, &box->type)) {
goto error;
}
boxinfo = jp2_boxinfolookup(box->type);
box->info = boxinfo;
	box->ops = &boxinfo->ops;
box->len = len;
JAS_DBGLOG(10, (
"preliminary processing of JP2 box: type=%c%s%c (0x%08x); length=%d\n",
'"', boxinfo->name, '"', box->type, box->len
));
if (box->len == 1) {
if (jp2_getuint64(in, &extlen)) {
goto error;
}
if (extlen > 0xffffffffUL) {
jas_eprintf("warning: cannot handle large 64-bit box length\n");
extlen = 0xffffffffUL;
}
box->len = extlen;
box->datalen = extlen - JP2_BOX_HDRLEN(true);
} else {
box->datalen = box->len - JP2_BOX_HDRLEN(false);
}
if (box->len != 0 && box->len < 8) {
goto error;
}

dataflag = !(box->info->flags & (JP2_BOX_SUPER | JP2_BOX_NODATA));

if (dataflag) {
if (!(tmpstream = jas_stream_memopen(0, 0))) {
goto error;
}
if (jas_stream_copy(tmpstream, in, box->datalen)) {
			// Mark the box data as never having been constructed
			// so that we will not errantly attempt to destroy it later.
			box->ops = &jp2_boxinfo_unk.ops;
jas_eprintf("cannot copy box data\n");
goto error;
}
jas_stream_rewind(tmpstream);

if (box->ops->getdata) {
if ((*box->ops->getdata)(box, tmpstream)) {
jas_eprintf("cannot parse box data\n");
goto error;
}
}
jas_stream_close(tmpstream);
}

if (jas_getdbglevel() >= 1) {
jp2_box_dump(box, stderr);
}

return box;

error:
if (box) {
jp2_box_destroy(box);
}
if (tmpstream) {
jas_stream_close(tmpstream);
}
return 0;
}
