virtual bool InputMethodIsActivated(const std::string& input_method_id) {
    scoped_ptr<InputMethodDescriptors> active_input_method_descriptors(
        GetActiveInputMethods());
for (size_t i = 0; i < active_input_method_descriptors->size(); ++i) {
if (active_input_method_descriptors->at(i).id == input_method_id) {
return true;
}
}
return false;
}
