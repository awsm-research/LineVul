TemplateURLRef::SearchTermsArgs::ContextualSearchParams::ContextualSearchParams(
int version,
const std::string& selection,
const std::string& base_page_url,
    int now_on_tap_version)
: version(version),
start(base::string16::npos),
end(base::string16::npos),
selection(selection),
base_page_url(base_page_url),
      now_on_tap_version(now_on_tap_version) {}
