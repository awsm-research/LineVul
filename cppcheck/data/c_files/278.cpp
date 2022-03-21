std::string GetUploadData(const std::string& brand) {
DCHECK(!brand.empty());
  std::string data(kPostXml);
  const std::string placeholder("__BRANDCODE_PLACEHOLDER__");
  size_t placeholder_pos = data.find(placeholder);
  DCHECK(placeholder_pos != std::string::npos);
  data.replace(placeholder_pos, placeholder.size(), brand);
  return data;
}
