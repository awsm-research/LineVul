UpdateLibrary* CrosLibrary::GetUpdateLibrary() {
  return update_lib_.GetDefaultImpl(use_stub_impl_);
}
