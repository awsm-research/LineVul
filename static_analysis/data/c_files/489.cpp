void DistillerNativeJavaScript::EnsureServiceConnected() {
  if (!distiller_js_service_) {
render_frame_->GetServiceRegistry()->ConnectToRemoteService(
mojo::GetProxy(&distiller_js_service_));
}
}
