void CompileFromResponseCallback(
const v8::FunctionCallbackInfo<v8::Value>& args) {
ExceptionState exception_state(args.GetIsolate(),
ExceptionState::kExecutionContext,
"WebAssembly", "compile");
ExceptionToRejectPromiseScope reject_promise_scope(args, exception_state);

  ScriptState* script_state = ScriptState::ForRelevantRealm(args);
if (!ExecutionContext::From(script_state)) {
V8SetReturnValue(args, ScriptPromise().V8Value());
return;
}

if (args.Length() < 1 || !args[0]->IsObject() ||
!V8Response::hasInstance(args[0], args.GetIsolate())) {
V8SetReturnValue(
args,
ScriptPromise::Reject(
script_state, V8ThrowException::CreateTypeError(
script_state->GetIsolate(),
                              "An argument must be provided, which must be a"
"Response or Promise<Response> object"))
.V8Value());
return;
}

Response* response = V8Response::ToImpl(v8::Local<v8::Object>::Cast(args[0]));
if (response->MimeType() != "application/wasm") {
V8SetReturnValue(
args,
ScriptPromise::Reject(
script_state,
V8ThrowException::CreateTypeError(
script_state->GetIsolate(),
"Incorrect response MIME type. Expected 'application/wasm'."))
.V8Value());
return;
}
v8::Local<v8::Value> promise;
if (response->IsBodyLocked() || response->bodyUsed()) {
promise = ScriptPromise::Reject(script_state,
V8ThrowException::CreateTypeError(
script_state->GetIsolate(),
"Cannot compile WebAssembly.Module "
"from an already read Response"))
.V8Value();
} else {
if (response->BodyBuffer()) {
FetchDataLoaderAsWasmModule* loader =
new FetchDataLoaderAsWasmModule(script_state);

promise = loader->GetPromise();
response->BodyBuffer()->StartLoading(loader, new WasmDataLoaderClient());
} else {
promise = ScriptPromise::Reject(script_state,
V8ThrowException::CreateTypeError(
script_state->GetIsolate(),
"Response object has a null body."))
.V8Value();
}
}
V8SetReturnValue(args, promise);
}
