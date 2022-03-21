static void locationWithCallWithAttributeSetter(v8::Local<v8::Value> jsValue, const v8::PropertyCallbackInfo<void>& info)
{
TestObjectPython* proxyImp = V8TestObjectPython::toNative(info.Holder());
    TestNode* imp = WTF::getPtr(proxyImp->locationWithCallWith());
if (!imp)
return;
V8TRYCATCH_FOR_V8STRINGRESOURCE_VOID(V8StringResource<>, cppValue, jsValue);
imp->setHrefCallWith(callingDOMWindow(info.GetIsolate()), enteredDOMWindow(info.GetIsolate()), cppValue);
}
