XGetDeviceControl(
register Display	*dpy,
XDevice		*dev,
int			 control)
{
XDeviceControl *Device = NULL;
XDeviceControl *Sav = NULL;
xDeviceState *d = NULL;
xDeviceState *sav = NULL;
xGetDeviceControlReq *req;
xGetDeviceControlReply rep;
XExtDisplayInfo *info = XInput_find_display(dpy);

LockDisplay(dpy);
if (_XiCheckExtInit(dpy, XInput_Add_XChangeDeviceControl, info) == -1)
return NULL;

GetReq(GetDeviceControl, req);
req->reqType = info->codes->major_opcode;
req->ReqType = X_GetDeviceControl;
req->deviceid = dev->device_id;
req->control = control;

if (!_XReply(dpy, (xReply *) & rep, 0, xFalse))
goto out;

if (rep.length > 0) {
unsigned long nbytes;
size_t size = 0;
	if (rep.length < (INT_MAX >> 2)) {
nbytes = (unsigned long) rep.length << 2;
d = Xmalloc(nbytes);
}
_XEatDataWords(dpy, rep.length);
goto out;
}
sav = d;
_XRead(dpy, (char *)d, nbytes);

/* In theory, we should just be able to use d->length to get the size.
* Turns out that a number of X servers (up to and including server
* 1.4) sent the wrong length value down the wire. So to not break
* apps that run against older servers, we have to calculate the size
* manually.
*/
switch (d->control) {
case DEVICE_RESOLUTION:
{
xDeviceResolutionState *r;
size_t val_size;
size_t val_size;

r = (xDeviceResolutionState *) d;
	    if (r->num_valuators >= (INT_MAX / (3 * sizeof(int))))
goto out;
val_size = 3 * sizeof(int) * r->num_valuators;
if ((sizeof(xDeviceResolutionState) + val_size) > nbytes)
break;
}
case DEVICE_ABS_CALIB:
{
if (sizeof(xDeviceAbsCalibState) > nbytes)
goto out;
size = sizeof(XDeviceAbsCalibState);
break;
}
case DEVICE_ABS_AREA:
{
if (sizeof(xDeviceAbsAreaState) > nbytes)
goto out;
size = sizeof(XDeviceAbsAreaState);
break;
}
case DEVICE_CORE:
{
if (sizeof(xDeviceCoreState) > nbytes)
goto out;
size = sizeof(XDeviceCoreState);
break;
}
default:
if (d->length > nbytes)
goto out;
size = d->length;
break;
}

Device = Xmalloc(size);
if (!Device)
goto out;

Sav = Device;

d = sav;
switch (control) {
case DEVICE_RESOLUTION:
{
int *iptr, *iptr2;
xDeviceResolutionState *r;
XDeviceResolutionState *R;
unsigned int i;

r = (xDeviceResolutionState *) d;
R = (XDeviceResolutionState *) Device;

R->control = DEVICE_RESOLUTION;
R->length = sizeof(XDeviceResolutionState);
R->num_valuators = r->num_valuators;
iptr = (int *)(R + 1);
iptr2 = (int *)(r + 1);
R->resolutions = iptr;
R->min_resolutions = iptr + R->num_valuators;
R->max_resolutions = iptr + (2 * R->num_valuators);
for (i = 0; i < (3 * R->num_valuators); i++)
*iptr++ = *iptr2++;
break;
}
case DEVICE_ABS_CALIB:
{
xDeviceAbsCalibState *c = (xDeviceAbsCalibState *) d;
XDeviceAbsCalibState *C = (XDeviceAbsCalibState *) Device;

C->control = DEVICE_ABS_CALIB;
C->length = sizeof(XDeviceAbsCalibState);
C->min_x = c->min_x;
C->max_x = c->max_x;
C->min_y = c->min_y;
C->max_y = c->max_y;
C->flip_x = c->flip_x;
C->flip_y = c->flip_y;
C->rotation = c->rotation;
C->button_threshold = c->button_threshold;

break;
}
case DEVICE_ABS_AREA:
{
xDeviceAbsAreaState *a = (xDeviceAbsAreaState *) d;
XDeviceAbsAreaState *A = (XDeviceAbsAreaState *) Device;

A->control = DEVICE_ABS_AREA;
A->length = sizeof(XDeviceAbsAreaState);
A->offset_x = a->offset_x;
A->offset_y = a->offset_y;
A->width = a->width;
A->height = a->height;
A->screen = a->screen;
A->following = a->following;

break;
}
case DEVICE_CORE:
{
xDeviceCoreState *c = (xDeviceCoreState *) d;
XDeviceCoreState *C = (XDeviceCoreState *) Device;

C->control = DEVICE_CORE;
C->length = sizeof(XDeviceCoreState);
C->status = c->status;
C->iscore = c->iscore;

break;
}
case DEVICE_ENABLE:
{
xDeviceEnableState *e = (xDeviceEnableState *) d;
XDeviceEnableState *E = (XDeviceEnableState *) Device;

E->control = DEVICE_ENABLE;
E->length = sizeof(E);
E->enable = e->enable;

break;
}
default:
break;
}
}
