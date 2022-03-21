virtual void SetImePropertyActivated(const std::string& key,
bool activated) {
if (!initialized_successfully_)
return;

DCHECK(!key.empty());
    chromeos::SetImePropertyActivated(
        input_method_status_connection_, key.c_str(), activated);
}
