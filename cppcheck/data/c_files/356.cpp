bool TabsCaptureVisibleTabFunction::RunImpl() {
PrefService* service = profile()->GetPrefs();
if (service->GetBoolean(prefs::kDisableScreenshots)) {
error_ = keys::kScreenshotsDisabled;
return false;
}

WebContents* web_contents = NULL;
if (!GetTabToCapture(&web_contents))
return false;

image_format_ = FORMAT_JPEG;  // Default format is JPEG.
image_quality_ = kDefaultQuality;  // Default quality setting.

if (HasOptionalArgument(1)) {
DictionaryValue* options = NULL;
EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(1, &options));

if (options->HasKey(keys::kFormatKey)) {
std::string format;
EXTENSION_FUNCTION_VALIDATE(
options->GetString(keys::kFormatKey, &format));

if (format == keys::kFormatValueJpeg) {
image_format_ = FORMAT_JPEG;
} else if (format == keys::kFormatValuePng) {
image_format_ = FORMAT_PNG;
} else {
// Schema validation should make this unreachable.
EXTENSION_FUNCTION_VALIDATE(0);
}
}

if (options->HasKey(keys::kQualityKey)) {
EXTENSION_FUNCTION_VALIDATE(
options->GetInteger(keys::kQualityKey, &image_quality_));
}
}

  // captureVisibleTab() can return an image containing sensitive information
  // that the browser would otherwise protect.  Ensure the extension has
  // permission to do this.
  if (!GetExtension()->CanCaptureVisiblePage(
        web_contents->GetURL(),
        SessionID::IdForTab(web_contents),
        &error_)) {
return false;
}

RenderViewHost* render_view_host = web_contents->GetRenderViewHost();
content::RenderWidgetHostView* view = render_view_host->GetView();
if (!view) {
error_ = keys::kInternalVisibleTabCaptureError;
return false;
}
render_view_host->CopyFromBackingStore(
gfx::Rect(),
view->GetViewBounds().size(),
base::Bind(&TabsCaptureVisibleTabFunction::CopyFromBackingStoreComplete,
this));
return true;
}
