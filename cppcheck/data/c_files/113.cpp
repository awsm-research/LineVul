bool WebstoreStandaloneInstaller::EnsureUniqueInstall(
webstore_install::Result* reason,
std::string* error) {
InstallTracker* tracker = InstallTracker::Get(profile_);
DCHECK(tracker);

const ActiveInstallData* existing_install_data =
tracker->GetActiveInstall(id_);
if (existing_install_data) {
*reason = webstore_install::INSTALL_IN_PROGRESS;
    *error = kInstallInProgressError;
return false;
}

ActiveInstallData install_data(id_);
InitInstallData(&install_data);
scoped_active_install_.reset(new ScopedActiveInstall(tracker, install_data));
return true;
}
