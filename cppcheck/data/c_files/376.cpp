void CrosLibrary::TestApi::SetBrightnessLibrary(
    BrightnessLibrary* library, bool own) {
  library_->brightness_lib_.SetImpl(library, own);
}
