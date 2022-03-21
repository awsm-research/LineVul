NaClProcessHost::NaClProcessHost(const GURL& manifest_url, bool off_the_record)
: manifest_url_(manifest_url),
#if defined(OS_WIN)
process_launched_by_broker_(false),
#elif defined(OS_LINUX)
wait_for_nacl_gdb_(false),
#endif
reply_msg_(NULL),
#if defined(OS_WIN)
debug_exception_handler_requested_(false),
#endif
internal_(new NaClInternal()),
ALLOW_THIS_IN_INITIALIZER_LIST(weak_factory_(this)),
enable_exception_handling_(false),
off_the_record_(off_the_record) {
process_.reset(content::BrowserChildProcessHost::Create(
content::PROCESS_TYPE_NACL_LOADER, this));

// Set the display name so the user knows what plugin the process is running.
// We aren't on the UI thread so getting the pref locale for language
// formatting isn't possible, so IDN will be lost, but this is probably OK
// for this use case.
process_->SetName(net::FormatUrl(manifest_url_, std::string()));

// We allow untrusted hardware exception handling to be enabled via
// an env var for consistency with the standalone build of NaCl.
if (CommandLine::ForCurrentProcess()->HasSwitch(
switches::kEnableNaClExceptionHandling) ||
getenv("NACL_UNTRUSTED_EXCEPTION_HANDLING") != NULL) {
enable_exception_handling_ = true;
}
  enable_ipc_proxy_ = CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableNaClIPCProxy);
}
