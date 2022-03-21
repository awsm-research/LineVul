void BluetoothDeviceChooserController::GetDevice(
blink::mojom::WebBluetoothRequestDeviceOptionsPtr options,
const SuccessCallback& success_callback,
const ErrorCallback& error_callback) {
DCHECK_CURRENTLY_ON(BrowserThread::UI);

// GetDevice should only be called once.
DCHECK(success_callback_.is_null());
DCHECK(error_callback_.is_null());

success_callback_ = success_callback;
error_callback_ = error_callback;
options_ = std::move(options);
LogRequestDeviceOptions(options_);

// Check blocklist to reject invalid filters and adjust optional_services.
if (options_->filters &&
BluetoothBlocklist::Get().IsExcluded(options_->filters.value())) {
RecordRequestDeviceOutcome(
UMARequestDeviceOutcome::BLOCKLISTED_SERVICE_IN_FILTER);
    PostErrorCallback(
        blink::mojom::WebBluetoothResult::REQUEST_DEVICE_WITH_BLOCKLISTED_UUID);
return;
}
BluetoothBlocklist::Get().RemoveExcludedUUIDs(options_.get());

  const url::Origin requesting_origin =
      render_frame_host_->GetLastCommittedOrigin();
  const url::Origin embedding_origin =
      web_contents_->GetMainFrame()->GetLastCommittedOrigin();
  // TODO(crbug.com/518042): Enforce correctly-delegated permissions instead of
  // matching origins. When relaxing this, take care to handle non-sandboxed
  // unique origins.
  if (!embedding_origin.IsSameOriginWith(requesting_origin)) {
    PostErrorCallback(blink::mojom::WebBluetoothResult::
                          REQUEST_DEVICE_FROM_CROSS_ORIGIN_IFRAME);
return;
}
  // The above also excludes opaque origins, which are not even same-origin with
  // themselves.
  DCHECK(!requesting_origin.opaque());

if (!adapter_->IsPresent()) {
DVLOG(1) << "Bluetooth Adapter not present. Can't serve requestDevice.";
RecordRequestDeviceOutcome(
UMARequestDeviceOutcome::BLUETOOTH_ADAPTER_NOT_PRESENT);
    PostErrorCallback(blink::mojom::WebBluetoothResult::NO_BLUETOOTH_ADAPTER);
return;
}

  switch (GetContentClient()->browser()->AllowWebBluetooth(
      web_contents_->GetBrowserContext(), requesting_origin,
      embedding_origin)) {
    case ContentBrowserClient::AllowWebBluetoothResult::BLOCK_POLICY: {
      RecordRequestDeviceOutcome(
          UMARequestDeviceOutcome::BLUETOOTH_CHOOSER_POLICY_DISABLED);
      PostErrorCallback(blink::mojom::WebBluetoothResult::
                            CHOOSER_NOT_SHOWN_API_LOCALLY_DISABLED);
      return;
    }
    case ContentBrowserClient::AllowWebBluetoothResult::
        BLOCK_GLOBALLY_DISABLED: {
      // Log to the developer console.
      web_contents_->GetMainFrame()->AddMessageToConsole(
          blink::mojom::ConsoleMessageLevel::kInfo,
          "Bluetooth permission has been blocked.");
      // Block requests.
      RecordRequestDeviceOutcome(
          UMARequestDeviceOutcome::BLUETOOTH_GLOBALLY_DISABLED);
      PostErrorCallback(blink::mojom::WebBluetoothResult::
                            CHOOSER_NOT_SHOWN_API_GLOBALLY_DISABLED);
      return;
    }
    case ContentBrowserClient::AllowWebBluetoothResult::ALLOW:
      break;
  }
BluetoothChooser::EventHandler chooser_event_handler =
base::Bind(&BluetoothDeviceChooserController::OnBluetoothChooserEvent,
base::Unretained(this));

if (WebContentsDelegate* delegate = web_contents_->GetDelegate()) {
chooser_ = delegate->RunBluetoothChooser(render_frame_host_,
std::move(chooser_event_handler));
}

if (!chooser_.get()) {
    PostErrorCallback(
        blink::mojom::WebBluetoothResult::WEB_BLUETOOTH_NOT_SUPPORTED);
return;
}

if (!chooser_->CanAskForScanningPermission()) {
DVLOG(1) << "Closing immediately because Chooser cannot obtain permission.";
OnBluetoothChooserEvent(BluetoothChooser::Event::DENIED_PERMISSION,
"" /* device_address */);
return;
}

device_ids_.clear();
PopulateConnectedDevices();
if (!chooser_.get()) {
// If the dialog's closing, no need to do any of the rest of this.
return;
}

if (!adapter_->IsPowered()) {
chooser_->SetAdapterPresence(
BluetoothChooser::AdapterPresence::POWERED_OFF);
return;
}

StartDeviceDiscovery();
}
