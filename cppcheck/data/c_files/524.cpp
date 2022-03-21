ScreenLockLibrary* CrosLibrary::GetScreenLockLibrary() {
  return screen_lock_lib_.GetDefaultImpl(use_stub_impl_);
}
