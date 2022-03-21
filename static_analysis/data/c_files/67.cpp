static v8::Handle<v8::Value> methodWithCallbackArgCallback(const v8::Arguments& args)
{
INC_STATS("DOM.TestObj.methodWithCallbackArg");
if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
TestObj* imp = V8TestObj::toNative(args.Holder());
if (args.Length() <= 0 || !args[0]->IsFunction())
return throwError(TYPE_MISMATCH_ERR, args.GetIsolate());
RefPtr<TestCallback> callback = V8TestCallback::create(args[0], getScriptExecutionContext());
imp->methodWithCallbackArg(callback);
return v8::Handle<v8::Value>();
}
