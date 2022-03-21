bool RendererPermissionsPolicyDelegate::IsRestrictedUrl(
const GURL& document_url,
std::string* error) {
if (dispatcher_->IsExtensionActive(kWebStoreAppId)) {
if (error)
*error = errors::kCannotScriptGallery;
return true;
}

  if (SearchBouncer::GetInstance()->IsNewTabPage(document_url)) {
if (error)
*error = errors::kCannotScriptNtp;
return true;
}

return false;
}
