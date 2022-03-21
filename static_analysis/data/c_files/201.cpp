EncodedJSValue JSC_HOST_CALL JSTestObjConstructor::constructJSTestObj(ExecState* exec)
{
JSTestObjConstructor* castedThis = jsCast<JSTestObjConstructor*>(exec->callee());
if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
if (exec->argumentCount() <= 0 || !exec->argument(0).isFunction()) {
setDOMException(exec, TYPE_MISMATCH_ERR);
return JSValue::encode(jsUndefined());
}
RefPtr<TestCallback> testCallback = JSTestCallback::create(asObject(exec->argument(0)), castedThis->globalObject());
RefPtr<TestObj> object = TestObj::create(testCallback);
return JSValue::encode(asObject(toJS(exec, castedThis->globalObject(), object.get())));
}
