WebContents* TabsCaptureVisibleTabFunction::GetWebContentsForID(
int window_id,
std::string* error) {
Browser* browser = NULL;
if (!GetBrowserFromWindowID(chrome_details_, window_id, &browser, error))
return nullptr;

WebContents* contents = browser->tab_strip_model()->GetActiveWebContents();
if (!contents) {
*error = "No active web contents to capture";
return nullptr;
}

if (!extension()->permissions_data()->CanCaptureVisiblePage(
contents->GetLastCommittedURL(),
          SessionTabHelper::IdForTab(contents).id(), error)) {
return nullptr;
}
return contents;
}
