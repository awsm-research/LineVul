void AppShortcutManager::OnceOffCreateShortcuts() {
  bool was_enabled = prefs_->GetBoolean(prefs::kAppShortcutsHaveBeenCreated);
  // Creation of shortcuts on Mac currently can be disabled with
  // --disable-app-shims, so check the flag, and set the pref accordingly.
#if defined(OS_MACOSX)
  bool is_now_enabled = apps::IsAppShimsEnabled();
#else
  bool is_now_enabled = true;
#endif  // defined(OS_MACOSX)
  if (was_enabled != is_now_enabled)
    prefs_->SetBoolean(prefs::kAppShortcutsHaveBeenCreated, is_now_enabled);
  if (was_enabled || !is_now_enabled)
return;

// Check if extension system/service are available. They might not be in
// tests.
extensions::ExtensionSystem* extension_system;
ExtensionServiceInterface* extension_service;
if (!(extension_system = extensions::ExtensionSystem::Get(profile_)) ||
!(extension_service = extension_system->extension_service()))
return;

// Create an applications menu shortcut for each app in this profile.
const extensions::ExtensionSet* apps = extension_service->extensions();
for (extensions::ExtensionSet::const_iterator it = apps->begin();
it != apps->end(); ++it) {
if (ShouldCreateShortcutFor(profile_, it->get()))
CreateShortcutsInApplicationsMenu(profile_, it->get());
}
}
