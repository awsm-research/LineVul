static void LongOrNullAttributeAttributeSetter(
v8::Local<v8::Value> v8_value, const v8::FunctionCallbackInfo<v8::Value>& info) {
v8::Isolate* isolate = info.GetIsolate();
ALLOW_UNUSED_LOCAL(isolate);

v8::Local<v8::Object> holder = info.Holder();
ALLOW_UNUSED_LOCAL(holder);

TestObject* impl = V8TestObject::ToImpl(holder);

ExceptionState exception_state(isolate, ExceptionState::kSetterContext, "TestObject", "longOrNullAttribute");

// Prepare the value to be set.
  int32_t cpp_value = NativeValueTraits<IDLLong>::NativeValue(info.GetIsolate(), v8_value, exception_state);
if (exception_state.HadException())
return;

  bool is_null = IsUndefinedOrNull(v8_value);
impl->setLongOrNullAttribute(cpp_value, is_null);
}
