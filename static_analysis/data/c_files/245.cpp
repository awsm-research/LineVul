void WebPluginDelegateProxy::OnGetWindowScriptNPObject(
    int route_id, bool* success, intptr_t* npobject_ptr) {
*success = false;
NPObject* npobject = NULL;
if (plugin_)
npobject = plugin_->GetWindowScriptNPObject();

if (!npobject)
return;

// The stub will delete itself when the proxy tells it that it's released, or
// otherwise when the channel is closed.
window_script_object_ = (new NPObjectStub(
npobject, channel_host_.get(), route_id, 0, page_url_))->AsWeakPtr();
*success = true;
  *npobject_ptr = reinterpret_cast<intptr_t>(npobject);
}
