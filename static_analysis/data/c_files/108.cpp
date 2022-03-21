XGetModifierMapping(register Display *dpy)
{
xGetModifierMappingReply rep;
register xReq *req;
unsigned long nbytes;
XModifierKeymap *res;

LockDisplay(dpy);
GetEmptyReq(GetModifierMapping, req);
(void) _XReply (dpy, (xReply *)&rep, 0, xFalse);

    if (rep.length < (INT_MAX >> 2)) {
nbytes = (unsigned long)rep.length << 2;
res = Xmalloc(sizeof (XModifierKeymap));
if (res)
} else
res = NULL;
if ((! res) || (! res->modifiermap)) {
Xfree(res);
res = (XModifierKeymap *) NULL;
_XEatDataWords(dpy, rep.length);
} else {
_XReadPad(dpy, (char *) res->modifiermap, (long) nbytes);
res->max_keypermod = rep.numKeyPerModifier;
}

UnlockDisplay(dpy);
SyncHandle();
return (res);
}
