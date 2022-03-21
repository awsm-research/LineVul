v8::Local<v8::Value> V8Debugger::generatorObjectLocation(v8::Local<v8::Object> object)
{
if (!enabled()) {
NOTREACHED();
return v8::Null(m_isolate);
}
v8::Local<v8::Value> argv[] = { object };
v8::Local<v8::Value> location = callDebuggerMethod("getGeneratorObjectLocation", 1, argv).ToLocalChecked();
    if (!location->IsObject())
return v8::Null(m_isolate);
    v8::Local<v8::Context> context = m_debuggerContext.Get(m_isolate);
    if (!markAsInternal(context, v8::Local<v8::Object>::Cast(location), V8InternalValueType::kLocation))
return v8::Null(m_isolate);
    return location;
}
