std::string MasterPreferences::GetCompressedVariationsSeed() const {
  return ExtractPrefString(prefs::kVariationsCompressedSeed);
}
