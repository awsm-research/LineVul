static void logi_dj_recv_add_djhid_device(struct dj_receiver_dev *djrcv_dev,
struct dj_report *dj_report)
{
/* Called in delayed work context */
struct hid_device *djrcv_hdev = djrcv_dev->hdev;
struct usb_interface *intf = to_usb_interface(djrcv_hdev->dev.parent);
struct usb_device *usbdev = interface_to_usbdev(intf);
struct hid_device *dj_hiddev;
struct dj_device *dj_dev;

/* Device index goes from 1 to 6, we need 3 bytes to store the
* semicolon, the index, and a null terminator
*/
unsigned char tmpstr[3];

if (dj_report->report_params[DEVICE_PAIRED_PARAM_SPFUNCTION] &
SPFUNCTION_DEVICE_LIST_EMPTY) {
dbg_hid("%s: device list is empty\n", __func__);
djrcv_dev->querying_devices = false;
return;
}

	if ((dj_report->device_index < DJ_DEVICE_INDEX_MIN) ||
	    (dj_report->device_index > DJ_DEVICE_INDEX_MAX)) {
		dev_err(&djrcv_hdev->dev, "%s: invalid device index:%d\n",
			__func__, dj_report->device_index);
		return;
	}
if (djrcv_dev->paired_dj_devices[dj_report->device_index]) {
/* The device is already known. No need to reallocate it. */
dbg_hid("%s: device is already known\n", __func__);
return;
}

dj_hiddev = hid_allocate_device();
if (IS_ERR(dj_hiddev)) {
dev_err(&djrcv_hdev->dev, "%s: hid_allocate_device failed\n",
__func__);
return;
}

dj_hiddev->ll_driver = &logi_dj_ll_driver;

dj_hiddev->dev.parent = &djrcv_hdev->dev;
dj_hiddev->bus = BUS_USB;
dj_hiddev->vendor = le16_to_cpu(usbdev->descriptor.idVendor);
dj_hiddev->product = le16_to_cpu(usbdev->descriptor.idProduct);
snprintf(dj_hiddev->name, sizeof(dj_hiddev->name),
"Logitech Unifying Device. Wireless PID:%02x%02x",
dj_report->report_params[DEVICE_PAIRED_PARAM_EQUAD_ID_MSB],
dj_report->report_params[DEVICE_PAIRED_PARAM_EQUAD_ID_LSB]);

usb_make_path(usbdev, dj_hiddev->phys, sizeof(dj_hiddev->phys));
snprintf(tmpstr, sizeof(tmpstr), ":%d", dj_report->device_index);
strlcat(dj_hiddev->phys, tmpstr, sizeof(dj_hiddev->phys));

dj_dev = kzalloc(sizeof(struct dj_device), GFP_KERNEL);

if (!dj_dev) {
dev_err(&djrcv_hdev->dev, "%s: failed allocating dj_device\n",
__func__);
goto dj_device_allocate_fail;
}

dj_dev->reports_supported = get_unaligned_le32(
dj_report->report_params + DEVICE_PAIRED_RF_REPORT_TYPE);
dj_dev->hdev = dj_hiddev;
dj_dev->dj_receiver_dev = djrcv_dev;
dj_dev->device_index = dj_report->device_index;
dj_hiddev->driver_data = dj_dev;

djrcv_dev->paired_dj_devices[dj_report->device_index] = dj_dev;

if (hid_add_device(dj_hiddev)) {
dev_err(&djrcv_hdev->dev, "%s: failed adding dj_device\n",
__func__);
goto hid_add_device_fail;
}

return;

hid_add_device_fail:
djrcv_dev->paired_dj_devices[dj_report->device_index] = NULL;
kfree(dj_dev);
dj_device_allocate_fail:
hid_destroy_device(dj_hiddev);
}
