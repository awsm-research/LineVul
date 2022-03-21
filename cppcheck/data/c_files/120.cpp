static v8::Handle<v8::Value> enabledAtRuntimeMethod2Callback(const v8::Arguments& args)
{
INC_STATS("DOM.TestObj.enabledAtRuntimeMethod2");
if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
TestObj* imp = V8TestObj::toNative(args.Holder());
EXCEPTION_BLOCK(int, intArg, V8int::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8int::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
imp->enabledAtRuntimeMethod2(intArg);
return v8::Handle<v8::Value>();
}
