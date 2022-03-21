static int usb_enumerate_device_otg(struct usb_device *udev)
{
int err = 0;

#ifdef	CONFIG_USB_OTG
/*
* OTG-aware devices on OTG-capable root hubs may be able to use SRP,
* to wake us after we've powered off VBUS; and HNP, switching roles
* "host" to "peripheral".  The OTG descriptor helps figure this out.
*/
if (!udev->bus->is_b_host
&& udev->config
&& udev->parent == udev->bus->root_hub) {
struct usb_otg_descriptor	*desc = NULL;
struct usb_bus			*bus = udev->bus;
unsigned			port1 = udev->portnum;

/* descriptor may appear anywhere in config */
err = __usb_get_extra_descriptor(udev->rawdescriptors[0],
le16_to_cpu(udev->config[0].desc.wTotalLength),
				USB_DT_OTG, (void **) &desc);
if (err || !(desc->bmAttributes & USB_OTG_HNP))
return 0;

dev_info(&udev->dev, "Dual-Role OTG device on %sHNP port\n",
(port1 == bus->otg_port) ? "" : "non-");

/* enable HNP before suspend, it's simpler */
if (port1 == bus->otg_port) {
bus->b_hnp_enable = 1;
err = usb_control_msg(udev,
usb_sndctrlpipe(udev, 0),
USB_REQ_SET_FEATURE, 0,
USB_DEVICE_B_HNP_ENABLE,
0, NULL, 0,
USB_CTRL_SET_TIMEOUT);
if (err < 0) {
/*
* OTG MESSAGE: report errors here,
* customize to match your product.
*/
dev_err(&udev->dev, "can't set HNP mode: %d\n",
err);
bus->b_hnp_enable = 0;
}
} else if (desc->bLength == sizeof
(struct usb_otg_descriptor)) {
/* Set a_alt_hnp_support for legacy otg device */
err = usb_control_msg(udev,
usb_sndctrlpipe(udev, 0),
USB_REQ_SET_FEATURE, 0,
USB_DEVICE_A_ALT_HNP_SUPPORT,
0, NULL, 0,
USB_CTRL_SET_TIMEOUT);
if (err < 0)
dev_err(&udev->dev,
"set a_alt_hnp_support failed: %d\n",
err);
}
}
#endif
return err;
}
