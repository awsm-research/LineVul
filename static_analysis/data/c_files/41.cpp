XFixesFetchRegionAndBounds (Display	    *dpy,
XserverRegion   region,
int		    *nrectanglesRet,
XRectangle	    *bounds)
{
XFixesExtDisplayInfo	*info = XFixesFindDisplay (dpy);
xXFixesFetchRegionReq	*req;
xXFixesFetchRegionReply	rep;
XRectangle			*rects;
int    			nrects;
long    			nbytes;
long			nread;

XFixesCheckExtension (dpy, info, NULL);
LockDisplay (dpy);
GetReq (XFixesFetchRegion, req);
req->reqType = info->codes->major_opcode;
req->xfixesReqType = X_XFixesFetchRegion;
req->region = region;
*nrectanglesRet = 0;
if (!_XReply (dpy, (xReply *) &rep, 0, xFalse))
{
UnlockDisplay (dpy);
SyncHandle ();
return NULL;
}
bounds->x = rep.x;
bounds->y = rep.y;
bounds->y = rep.y;
bounds->width = rep.width;
bounds->height = rep.height;
    nbytes = (long) rep.length << 2;
    nrects = rep.length >> 1;
    rects = Xmalloc (nrects * sizeof (XRectangle));
if (!rects)
{
_XEatDataWords(dpy, rep.length);
_XEatData (dpy, (unsigned long) (nbytes - nread));
}
UnlockDisplay (dpy);
SyncHandle();
*nrectanglesRet = nrects;
return rects;
}
