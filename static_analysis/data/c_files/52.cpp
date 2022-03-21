bool ParseRequestInfo(const struct mg_request_info* const request_info,
std::string* method,
std::vector<std::string>* path_segments,
DictionaryValue** parameters,
Response* const response) {
*method = request_info->request_method;
if (*method == "HEAD")
*method = "GET";
else if (*method == "PUT")
*method = "POST";

std::string uri(request_info->uri);
SessionManager* manager = SessionManager::GetInstance();
uri = uri.substr(manager->url_base().length());

base::SplitString(uri, '/', path_segments);

if (*method == "POST" && request_info->post_data_len > 0) {
    VLOG(1) << "...parsing request body";
std::string json(request_info->post_data, request_info->post_data_len);
    std::string error;
    if (!ParseJSONDictionary(json, parameters, &error)) {
response->SetError(new Error(
kBadRequest,
          "Failed to parse command data: " + error + "\n  Data: " + json));
return false;
}
}
  VLOG(1) << "Parsed " << method << " " << uri
        << std::string(request_info->post_data, request_info->post_data_len);
return true;
}
