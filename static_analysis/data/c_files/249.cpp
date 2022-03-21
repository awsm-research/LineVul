v8::Handle<v8::Value> V8XMLHttpRequest::openCallback(const v8::Arguments& args)
{
INC_STATS("DOM.XMLHttpRequest.open()");
// Four cases:
// open(method, url)
// open(method, url, async)
// open(method, url, async, user)
// open(method, url, async, user, passwd)

if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError();

XMLHttpRequest* xmlHttpRequest = V8XMLHttpRequest::toNative(args.Holder());

String method = toWebCoreString(args[0]);
String urlstring = toWebCoreString(args[1]);
ScriptExecutionContext* context = getScriptExecutionContext();
if (!context)
return v8::Undefined();

KURL url = context->completeURL(urlstring);

ExceptionCode ec = 0;

if (args.Length() >= 3) {
bool async = args[2]->BooleanValue();

if (args.Length() >= 4 && !args[3]->IsUndefined()) {
String user = toWebCoreStringWithNullCheck(args[3]);

if (args.Length() >= 5 && !args[4]->IsUndefined()) {
String passwd = toWebCoreStringWithNullCheck(args[4]);
xmlHttpRequest->open(method, url, async, user, passwd, ec);
} else
xmlHttpRequest->open(method, url, async, user, ec);
} else
xmlHttpRequest->open(method, url, async, ec);
} else
xmlHttpRequest->open(method, url, ec);

if (ec)
return throwError(ec, args.GetIsolate());

return v8::Undefined();
}
