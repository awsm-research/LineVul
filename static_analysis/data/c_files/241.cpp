bool IsTraceEventArgsWhitelisted(const char* category_group_name,
                                 const char* event_name) {
if (base::MatchPattern(category_group_name, "toplevel") &&
base::MatchPattern(event_name, "*")) {
return true;
}

return false;
}
