EncodedJSValue JSC_HOST_CALL jsTestInterfacePrototypeFunctionSupplementalMethod2(ExecState* exec)
{
JSValue thisValue = exec->hostThisValue();
if (!thisValue.inherits(&JSTestInterface::s_info))
return throwVMTypeError(exec);
JSTestInterface* castedThis = jsCast<JSTestInterface*>(asObject(thisValue));
ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestInterface::s_info);
TestInterface* impl = static_cast<TestInterface*>(castedThis->impl());
if (exec->argumentCount() < 2)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
ExceptionCode ec = 0;
ScriptExecutionContext* scriptContext = jsCast<JSDOMGlobalObject*>(exec->lexicalGlobalObject())->scriptExecutionContext();
if (!scriptContext)
return JSValue::encode(jsUndefined());
const String& strArg(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
if (exec->hadException())
return JSValue::encode(jsUndefined());
TestObj* objArg(toTestObj(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined)));
if (exec->hadException())
return JSValue::encode(jsUndefined());

JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(TestSupplemental::supplementalMethod2(impl, scriptContext, strArg, objArg, ec)));
setDOMException(exec, ec);
return JSValue::encode(result);
}
