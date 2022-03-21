GURL DecorateFrontendURL(const GURL& base_url) {
std::string frontend_url = base_url.spec();
std::string url_string(
frontend_url +
((frontend_url.find("?") == std::string::npos) ? "?" : "&") +
"dockSide=undocked"); // TODO(dgozman): remove this support in M38.
base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
if (command_line->HasSwitch(switches::kEnableDevToolsExperiments))
url_string += "&experiments=true";

if (command_line->HasSwitch(switches::kDevToolsFlags)) {
    std::string flags = command_line->GetSwitchValueASCII(
                            switches::kDevToolsFlags);
    flags = net::EscapeQueryParamValue(flags, false);
    url_string += "&flags=" + flags;
}

#if defined(DEBUG_DEVTOOLS)
url_string += "&debugFrontend=true";
#endif  // defined(DEBUG_DEVTOOLS)

return GURL(url_string);
}
