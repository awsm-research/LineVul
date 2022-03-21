static int hid_abort_transfers(int sub_api, struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer = USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct windows_transfer_priv *transfer_priv = (struct windows_transfer_priv*)usbi_transfer_get_os_priv(itransfer);
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(transfer->dev_handle);
	HANDLE hid_handle;
	int current_interface;
	CHECK_HID_AVAILABLE;
	current_interface = transfer_priv->interface_number;
	hid_handle = handle_priv->interface_handle[current_interface].api_handle;
	CancelIo(hid_handle);
	return LIBUSB_SUCCESS;
}
