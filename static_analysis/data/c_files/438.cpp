void ServiceWorkerPaymentInstrument::OnPaymentAppInvoked(
mojom::PaymentHandlerResponsePtr response) {
  DCHECK(delegate_);
if (delegate_ != nullptr) {
delegate_->OnInstrumentDetailsReady(response->method_name,
response->stringified_details);
delegate_ = nullptr;
}
}
