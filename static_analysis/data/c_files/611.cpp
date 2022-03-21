static int auto_claim(struct libusb_transfer *transfer, int *interface_number, int api_type)
{
struct libusb_context *ctx = DEVICE_CTX(transfer->dev_handle->dev);
struct windows_device_handle_priv *handle_priv = _device_handle_priv(
transfer->dev_handle);
struct windows_device_priv *priv = _device_priv(transfer->dev_handle->dev);
int current_interface = *interface_number;
int r = LIBUSB_SUCCESS;

switch(api_type) {
case USB_API_WINUSBX:
	case USB_API_HID:
break;
default:
return LIBUSB_ERROR_INVALID_PARAM;
}

usbi_mutex_lock(&autoclaim_lock);
if (current_interface < 0)	// No serviceable interface was found
{
for (current_interface=0; current_interface<USB_MAXINTERFACES; current_interface++) {
// Must claim an interface of the same API type
if ( (priv->usb_interface[current_interface].apib->id == api_type)
&& (libusb_claim_interface(transfer->dev_handle, current_interface) == LIBUSB_SUCCESS) ) {
usbi_dbg("auto-claimed interface %d for control request", current_interface);
if (handle_priv->autoclaim_count[current_interface] != 0) {
usbi_warn(ctx, "program assertion failed - autoclaim_count was nonzero");
}
handle_priv->autoclaim_count[current_interface]++;
break;
}
}
if (current_interface == USB_MAXINTERFACES) {
usbi_err(ctx, "could not auto-claim any interface");
r = LIBUSB_ERROR_NOT_FOUND;
}
} else {
// If we have a valid interface that was autoclaimed, we must increment
// its autoclaim count so that we can prevent an early release.
if (handle_priv->autoclaim_count[current_interface] != 0) {
handle_priv->autoclaim_count[current_interface]++;
}
}
usbi_mutex_unlock(&autoclaim_lock);

*interface_number = current_interface;
return r;

}
