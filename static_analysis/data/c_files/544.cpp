static v8::Handle<v8::Value> acceptTransferListCallback(const v8::Arguments& args)
{
INC_STATS("DOM.TestSerializedScriptValueInterface.acceptTransferList");
if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
TestSerializedScriptValueInterface* imp = V8TestSerializedScriptValueInterface::toNative(args.Holder());
MessagePortArray messagePortArrayTransferList;
ArrayBufferArray arrayBufferArrayTransferList;
if (args.Length() > 1) {
if (!extractTransferables(args[1], messagePortArrayTransferList, arrayBufferArrayTransferList))
return V8Proxy::throwTypeError("Could not extract transferables");
}
bool dataDidThrow = false;
RefPtr<SerializedScriptValue> data = SerializedScriptValue::create(args[0], &messagePortArrayTransferList, &arrayBufferArrayTransferList, dataDidThrow, args.GetIsolate());
if (dataDidThrow)
return v8::Undefined();
if (args.Length() <= 1) {
imp->acceptTransferList(data);
return v8::Handle<v8::Value>();
}
imp->acceptTransferList(data, messagePortArrayTransferList);
return v8::Handle<v8::Value>();
}
