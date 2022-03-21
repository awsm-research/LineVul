void CachingPermutedEntropyProvider::RegisterPrefs(
PrefRegistrySimple* registry) {
  registry->RegisterStringPref(prefs::kVariationsPermutedEntropyCache,
                               std::string());
}
