void CallbackAndDie(bool succeeded) {
    v8::Isolate* isolate = context_->isolate();
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Value> args[] = {v8::Boolean::New(isolate, succeeded)};
    context_->CallFunction(v8::Local<v8::Function>::New(isolate, callback_),
                           arraysize(args), args);
delete this;
}
