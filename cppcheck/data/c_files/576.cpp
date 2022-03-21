void EventBindings::AttachFilteredEvent(
const v8::FunctionCallbackInfo<v8::Value>& args) {
CHECK_EQ(2, args.Length());
CHECK(args[0]->IsString());
CHECK(args[1]->IsObject());

std::string event_name = *v8::String::Utf8Value(args[0]);
if (!context()->HasAccessOrThrowError(event_name))
return;

std::unique_ptr<base::DictionaryValue> filter;
{
std::unique_ptr<content::V8ValueConverter> converter(
content::V8ValueConverter::create());
std::unique_ptr<base::Value> filter_value(converter->FromV8Value(
v8::Local<v8::Object>::Cast(args[1]), context()->v8_context()));
if (!filter_value || !filter_value->IsType(base::Value::TYPE_DICTIONARY)) {
args.GetReturnValue().Set(static_cast<int32_t>(-1));
return;
}
filter = base::DictionaryValue::From(std::move(filter_value));
}

  // Hold onto a weak reference to |filter| so that it can be used after passing
  // ownership to |event_filter|.
  base::DictionaryValue* filter_weak = filter.get();
int id = g_event_filter.Get().AddEventMatcher(
event_name, ParseEventMatcher(std::move(filter)));
attached_matcher_ids_.insert(id);

// Only send IPCs the first time a filter gets added.
std::string extension_id = context()->GetExtensionID();
if (AddFilter(event_name, extension_id, *filter_weak)) {
bool lazy = ExtensionFrameHelper::IsContextForEventPage(context());
content::RenderThread::Get()->Send(new ExtensionHostMsg_AddFilteredListener(
extension_id, event_name, *filter_weak, lazy));
}

args.GetReturnValue().Set(static_cast<int32_t>(id));
}
