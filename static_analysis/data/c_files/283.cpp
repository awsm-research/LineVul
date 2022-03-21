BGD_DECLARE(void *) gdImageWebpPtrEx (gdImagePtr im, int *size, int quality)
{
void *rv;
gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
if (out == NULL) {
return NULL;
}
	gdImageWebpCtx(im, out, quality);
	rv = gdDPExtractData(out, size);
out->gd_free(out);
return rv;
}
