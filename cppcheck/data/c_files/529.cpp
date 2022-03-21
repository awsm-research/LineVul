static int dynamicGetbuf(gdIOCtxPtr ctx, void *buf, int len)
{
int rlen, remain;
dpIOCtxPtr dctx;
dynamicPtr *dp;

dctx = (dpIOCtxPtr) ctx;
dp = dctx->dp;

remain = dp->logicalSize - dp->pos;
if(remain >= len) {
rlen = len;
} else {
		if(remain == 0) {
/* 2.0.34: EOF is incorrect. We use 0 for
* errors and EOF, just like fileGetbuf,
* which is a simple fread() wrapper.
* TBB. Original bug report: Daniel Cowgill. */
return 0; /* NOT EOF */
}

rlen = remain;
}

memcpy(buf, (void *) ((char *)dp->data + dp->pos), rlen);
dp->pos += rlen;

return rlen;
}
