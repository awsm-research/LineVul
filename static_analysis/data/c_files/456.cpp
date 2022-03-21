void BluetoothDeviceChromeOS::RequestPinCode(
    const dbus::ObjectPath& device_path,
    const PinCodeCallback& callback) {
  DCHECK(agent_.get());
  DCHECK(device_path == object_path_);
  VLOG(1) << object_path_.value() << ": RequestPinCode";
  UMA_HISTOGRAM_ENUMERATION("Bluetooth.PairingMethod",
                            UMA_PAIRING_METHOD_REQUEST_PINCODE,
                            UMA_PAIRING_METHOD_COUNT);
  DCHECK(pairing_delegate_);
  DCHECK(pincode_callback_.is_null());
  pincode_callback_ = callback;
  pairing_delegate_->RequestPinCode(this);
  pairing_delegate_used_ = true;
}
