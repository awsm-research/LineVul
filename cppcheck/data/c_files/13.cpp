IDNConversionResult IDNToUnicodeWithAdjustmentsImpl(
base::StringPiece host,
base::OffsetAdjuster::Adjustments* adjustments,
bool enable_spoof_checks) {
if (adjustments)
adjustments->clear();
// Convert the ASCII input to a base::string16 for ICU.
base::string16 input16;
input16.reserve(host.length());
input16.insert(input16.end(), host.begin(), host.end());

  bool is_tld_ascii = true;
size_t last_dot = host.rfind('.');
  if (last_dot != base::StringPiece::npos &&
      host.substr(last_dot).starts_with(".xn--")) {
    is_tld_ascii = false;
}

IDNConversionResult result;
// Do each component of the host separately, since we enforce script matching
// on a per-component basis.
base::string16 out16;
for (size_t component_start = 0, component_end;
component_start < input16.length();
component_start = component_end + 1) {
// Find the end of the component.
component_end = input16.find('.', component_start);
if (component_end == base::string16::npos)
component_end = input16.length();  // For getting the last component.
size_t component_length = component_end - component_start;
size_t new_component_start = out16.length();
bool converted_idn = false;
if (component_end > component_start) {
// Add the substring that we just found.
bool has_idn_component = false;
converted_idn = IDNToUnicodeOneComponent(
          input16.data() + component_start, component_length, is_tld_ascii,
enable_spoof_checks, &out16, &has_idn_component);
result.has_idn_component |= has_idn_component;
}
size_t new_component_length = out16.length() - new_component_start;

if (converted_idn && adjustments) {
adjustments->push_back(base::OffsetAdjuster::Adjustment(
component_start, component_length, new_component_length));
}

// Need to add the dot we just found (if we found one).
if (component_end < input16.length())
out16.push_back('.');
}

result.result = out16;

// Leave as punycode any inputs that spoof top domains.
if (result.has_idn_component) {
result.matching_top_domain =
g_idn_spoof_checker.Get().GetSimilarTopDomain(out16);
if (enable_spoof_checks && !result.matching_top_domain.domain.empty()) {
if (adjustments)
adjustments->clear();
result.result = input16;
}
}

return result;
}
