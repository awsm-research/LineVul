void ObjectBackedNativeHandler::Router(
const v8::FunctionCallbackInfo<v8::Value>& args) {
v8::Isolate* isolate = args.GetIsolate();
v8::HandleScope handle_scope(isolate);
v8::Local<v8::Object> data = args.Data().As<v8::Object>();
v8::Local<v8::Context> context = isolate->GetCurrentContext();

v8::Local<v8::Value> handler_function_value;
v8::Local<v8::Value> feature_name_value;
// See comment in header file for why we do this.
if (!GetPrivate(context, data, kHandlerFunction, &handler_function_value) ||
handler_function_value->IsUndefined() ||
!GetPrivate(context, data, kFeatureName, &feature_name_value) ||
!feature_name_value->IsString()) {
ScriptContext* script_context =
ScriptContextSet::GetContextByV8Context(context);
console::Error(script_context ? script_context->GetRenderFrame() : nullptr,
"Extension view no longer exists");
return;
}

// We can't access the ScriptContextSet on a worker thread. Luckily, we also
// don't inject many bindings into worker threads.
// TODO(devlin): Figure out a way around this.
if (content::WorkerThread::GetCurrentId() == 0) {
ScriptContext* script_context =
ScriptContextSet::GetContextByV8Context(context);
v8::Local<v8::String> feature_name_string =
feature_name_value->ToString(context).ToLocalChecked();
std::string feature_name = *v8::String::Utf8Value(feature_name_string);
// TODO(devlin): Eventually, we should fail if either script_context is null
// or feature_name is empty.
    if (script_context &&
        !feature_name.empty() &&
        !script_context->GetAvailability(feature_name).is_available()) {
      return;
}
}
// This CHECK is *important*. Otherwise, we'll go around happily executing
// something random.  See crbug.com/548273.
CHECK(handler_function_value->IsExternal());
static_cast<HandlerFunction*>(
handler_function_value.As<v8::External>()->Value())->Run(args);

// Verify that the return value, if any, is accessible by the context.
v8::ReturnValue<v8::Value> ret = args.GetReturnValue();
v8::Local<v8::Value> ret_value = ret.Get();
if (ret_value->IsObject() && !ret_value->IsNull() &&
!ContextCanAccessObject(context, v8::Local<v8::Object>::Cast(ret_value),
true)) {
NOTREACHED() << "Insecure return value";
ret.SetUndefined();
}
}
