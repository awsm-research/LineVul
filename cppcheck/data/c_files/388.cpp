RenderFrameObserverNatives::RenderFrameObserverNatives(ScriptContext* context)
    : ObjectBackedNativeHandler(context) {
RouteFunction(
"OnDocumentElementCreated",
base::Bind(&RenderFrameObserverNatives::OnDocumentElementCreated,
base::Unretained(this)));
}
