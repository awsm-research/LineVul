get_endpoints(struct usbtest_dev *dev, struct usb_interface *intf)
{
int				tmp;
struct usb_host_interface	*alt;
struct usb_host_endpoint	*in, *out;
struct usb_host_endpoint	*iso_in, *iso_out;
struct usb_host_endpoint	*int_in, *int_out;
struct usb_device		*udev;

for (tmp = 0; tmp < intf->num_altsetting; tmp++) {
unsigned	ep;

in = out = NULL;
iso_in = iso_out = NULL;
int_in = int_out = NULL;
alt = intf->altsetting + tmp;

if (override_alt >= 0 &&
override_alt != alt->desc.bAlternateSetting)
continue;

/* take the first altsetting with in-bulk + out-bulk;
* ignore other endpoints and altsettings.
*/
for (ep = 0; ep < alt->desc.bNumEndpoints; ep++) {
struct usb_host_endpoint	*e;
int edi;

e = alt->endpoint + ep;
edi = usb_endpoint_dir_in(&e->desc);

switch (usb_endpoint_type(&e->desc)) {
case USB_ENDPOINT_XFER_BULK:
endpoint_update(edi, &in, &out, e);
continue;
case USB_ENDPOINT_XFER_INT:
if (dev->info->intr)
endpoint_update(edi, &int_in, &int_out, e);
continue;
case USB_ENDPOINT_XFER_ISOC:
if (dev->info->iso)
endpoint_update(edi, &iso_in, &iso_out, e);
/* FALLTHROUGH */
default:
continue;
}
}
if ((in && out)  ||  iso_in || iso_out || int_in || int_out)
goto found;
}
return -EINVAL;

found:
udev = testdev_to_usbdev(dev);
dev->info->alt = alt->desc.bAlternateSetting;
if (alt->desc.bAlternateSetting != 0) {
tmp = usb_set_interface(udev,
alt->desc.bInterfaceNumber,
alt->desc.bAlternateSetting);
if (tmp < 0)
return tmp;
}

	if (in) {
dev->in_pipe = usb_rcvbulkpipe(udev,
in->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK);
dev->out_pipe = usb_sndbulkpipe(udev,
out->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK);
	}
if (iso_in) {
dev->iso_in = &iso_in->desc;
dev->in_iso_pipe = usb_rcvisocpipe(udev,
iso_in->desc.bEndpointAddress
& USB_ENDPOINT_NUMBER_MASK);
}

if (iso_out) {
dev->iso_out = &iso_out->desc;
dev->out_iso_pipe = usb_sndisocpipe(udev,
iso_out->desc.bEndpointAddress
& USB_ENDPOINT_NUMBER_MASK);
}

if (int_in) {
dev->int_in = &int_in->desc;
dev->in_int_pipe = usb_rcvintpipe(udev,
int_in->desc.bEndpointAddress
& USB_ENDPOINT_NUMBER_MASK);
}

if (int_out) {
dev->int_out = &int_out->desc;
dev->out_int_pipe = usb_sndintpipe(udev,
int_out->desc.bEndpointAddress
& USB_ENDPOINT_NUMBER_MASK);
}
return 0;
}
