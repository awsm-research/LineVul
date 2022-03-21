static int atusb_get_and_show_revision(struct atusb *atusb)
{
struct usb_device *usb_dev = atusb->usb_dev;
	unsigned char buffer[3];
int ret;

/* Get a couple of the ATMega Firmware values */
ret = atusb_control_msg(atusb, usb_rcvctrlpipe(usb_dev, 0),
ATUSB_ID, ATUSB_REQ_FROM_DEV, 0, 0,
buffer, 3, 1000);
if (ret >= 0) {
atusb->fw_ver_maj = buffer[0];
atusb->fw_ver_min = buffer[1];
atusb->fw_hw_type = buffer[2];

dev_info(&usb_dev->dev,
"Firmware: major: %u, minor: %u, hardware type: %u\n",
atusb->fw_ver_maj, atusb->fw_ver_min, atusb->fw_hw_type);
}
if (atusb->fw_ver_maj == 0 && atusb->fw_ver_min < 2) {
dev_info(&usb_dev->dev,
"Firmware version (%u.%u) predates our first public release.",
atusb->fw_ver_maj, atusb->fw_ver_min);
dev_info(&usb_dev->dev, "Please update to version 0.2 or newer");
}

return ret;
}
