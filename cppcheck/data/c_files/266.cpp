v8::Handle<v8::Value> V8WebGLRenderingContext::getAttachedShadersCallback(const v8::Arguments& args)
{
INC_STATS("DOM.WebGLRenderingContext.getAttachedShaders()");

if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();

ExceptionCode ec = 0;
WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
if (args.Length() > 0 && !isUndefinedOrNull(args[0]) && !V8WebGLProgram::HasInstance(args[0])) {
V8Proxy::throwTypeError();
return notHandledByInterceptor();
}
WebGLProgram* program = V8WebGLProgram::HasInstance(args[0]) ? V8WebGLProgram::toNative(v8::Handle<v8::Object>::Cast(args[0])) : 0;
Vector<RefPtr<WebGLShader> > shaders;
bool succeed = context->getAttachedShaders(program, shaders, ec);
if (ec) {
V8Proxy::setDOMException(ec, args.GetIsolate());
return v8::Null();
}
if (!succeed)
return v8::Null();
v8::Local<v8::Array> array = v8::Array::New(shaders.size());
for (size_t ii = 0; ii < shaders.size(); ++ii)
array->Set(v8::Integer::New(ii), toV8(shaders[ii].get(), args.GetIsolate()));
return array;
}
