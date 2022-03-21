void HttpAuthFilterWhitelist::SetWhitelist(
const std::string& server_whitelist) {
  rules_.ParseFromString(server_whitelist);
}
