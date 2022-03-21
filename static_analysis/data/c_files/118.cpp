WebsiteSettingsPopupAndroid::WebsiteSettingsPopupAndroid(
JNIEnv* env,
jobject java_website_settings_pop,
content::WebContents* web_contents) {
// Important to use GetVisibleEntry to match what's showing in the omnibox.
content::NavigationEntry* nav_entry =
web_contents->GetController().GetVisibleEntry();
if (nav_entry == NULL)
return;

url_ = nav_entry->GetURL();

popup_jobject_.Reset(env, java_website_settings_pop);

presenter_.reset(new WebsiteSettings(
this,
Profile::FromBrowserContext(web_contents->GetBrowserContext()),
TabSpecificContentSettings::FromWebContents(web_contents),
      InfoBarService::FromWebContents(web_contents),
nav_entry->GetURL(),
nav_entry->GetSSL(),
content::CertStore::GetInstance()));
}
