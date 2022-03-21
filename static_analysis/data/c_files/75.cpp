void WebPluginImpl::didFinishLoadingFrameRequest(
const WebURL& url, void* notify_data) {
if (delegate_) {
delegate_->DidFinishLoadWithReason(
        url, NPRES_DONE, reinterpret_cast<intptr_t>(notify_data));
}
}
