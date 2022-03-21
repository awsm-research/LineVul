WebsiteSettings* website_settings() {
if (!website_settings_.get()) {
website_settings_.reset(new WebsiteSettings(
mock_ui(), profile(), tab_specific_content_settings(),
          infobar_service(), url(), ssl(), cert_store()));
}
return website_settings_.get();
}
