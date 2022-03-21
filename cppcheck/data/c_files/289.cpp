XIQueryDevice(Display *dpy, int deviceid, int *ndevices_return)
{
XIDeviceInfo        *info = NULL;
xXIQueryDeviceReq   *req;
xXIQueryDeviceReq   *req;
xXIQueryDeviceReply reply;
char                *ptr;
int                 i;
char                *buf;

LockDisplay(dpy);
if (_XiCheckExtInit(dpy, XInput_2_0, extinfo) == -1)
goto error_unlocked;

GetReq(XIQueryDevice, req);
req->reqType  = extinfo->codes->major_opcode;
req->ReqType  = X_XIQueryDevice;
req->deviceid = deviceid;

if (!_XReply(dpy, (xReply*) &reply, 0, xFalse))
goto error;
if (!_XReply(dpy, (xReply*) &reply, 0, xFalse))
goto error;

    *ndevices_return = reply.num_devices;
    info = Xmalloc((reply.num_devices + 1) * sizeof(XIDeviceInfo));
if (!info)
goto error;

buf = Xmalloc(reply.length * 4);
_XRead(dpy, buf, reply.length * 4);
ptr = buf;

/* info is a null-terminated array */
info[reply.num_devices].name = NULL;
nclasses         = wire->num_classes;

ptr += sizeof(xXIDeviceInfo);

lib->name = Xcalloc(wire->name_len + 1, 1);
XIDeviceInfo    *lib = &info[i];
xXIDeviceInfo   *wire = (xXIDeviceInfo*)ptr;

lib->deviceid    = wire->deviceid;
lib->use         = wire->use;
lib->attachment  = wire->attachment;

Xfree(buf);

ptr += sizeof(xXIDeviceInfo);

lib->name = Xcalloc(wire->name_len + 1, 1);
strncpy(lib->name, ptr, wire->name_len);
ptr += ((wire->name_len + 3)/4) * 4;

sz = size_classes((xXIAnyInfo*)ptr, nclasses);
lib->classes = Xmalloc(sz);
ptr += copy_classes(lib, (xXIAnyInfo*)ptr, &nclasses);
/* We skip over unused classes */
lib->num_classes = nclasses;
}
