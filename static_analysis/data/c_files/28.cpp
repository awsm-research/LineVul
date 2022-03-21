void EnableHighDPISupport() {
if (IsHighDPIEnabled() &&
      (base::win::GetVersion() < base::win::VERSION_WIN8_1)) {
    if (!SetProcessDpiAwarenessWrapper(PROCESS_SYSTEM_DPI_AWARE)) {
      SetProcessDPIAwareWrapper();
    }
}
}
