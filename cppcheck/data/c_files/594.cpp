void ExtensionServiceBackend::LoadSingleExtension(const FilePath& path_in) {
CHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));

FilePath extension_path = path_in;
file_util::AbsolutePath(&extension_path);

int flags = Extension::ShouldAlwaysAllowFileAccess(Extension::LOAD) ?
Extension::ALLOW_FILE_ACCESS : Extension::NO_FLAGS;
if (Extension::ShouldDoStrictErrorChecking(Extension::LOAD))
flags |= Extension::STRICT_ERROR_CHECKS;
std::string error;
scoped_refptr<const Extension> extension(extension_file_util::LoadExtension(
extension_path,
Extension::LOAD,
flags,
&error));

if (!extension) {
if (!BrowserThread::PostTask(
BrowserThread::UI, FROM_HERE,
NewRunnableMethod(
this,
&ExtensionServiceBackend::ReportExtensionLoadError,
extension_path, error)))
NOTREACHED() << error;
return;
}

// Report this as an installed extension so that it gets remembered in the
// prefs.
if (!BrowserThread::PostTask(
BrowserThread::UI, FROM_HERE,
NewRunnableMethod(
this,
              &ExtensionServiceBackend::OnExtensionInstalled,
extension)))
NOTREACHED();
}
