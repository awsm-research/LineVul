void UpdateNetworkManagerStatus() {
// Make sure we run on UI thread.
if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
BrowserThread::PostTask(
BrowserThread::UI, FROM_HERE,
NewRunnableMethod(this,
&NetworkLibraryImpl::UpdateNetworkManagerStatus));
return;
}

SystemInfo* system = GetSystemInfo();
if (!system)
return;


std::string prev_cellular_service_path = cellular_ ?
cellular_->service_path() : std::string();

ClearNetworks();

ParseSystem(system, &ethernet_, &wifi_networks_, &cellular_networks_,
&remembered_wifi_networks_);

wifi_ = NULL;
for (size_t i = 0; i < wifi_networks_.size(); i++) {
if (wifi_networks_[i]->connecting_or_connected()) {
wifi_ = wifi_networks_[i];
break;  // There is only one connected or connecting wifi network.
}
}
cellular_ = NULL;
for (size_t i = 0; i < cellular_networks_.size(); i++) {
if (cellular_networks_[i]->connecting_or_connected()) {
cellular_ = cellular_networks_[i];
        // If new cellular, then update data plan list.
if (cellular_networks_[i]->service_path() !=
prev_cellular_service_path) {
          CellularDataPlanList* list = RetrieveCellularDataPlans(
              cellular_->service_path().c_str());
          UpdateCellularDataPlan(list);
          FreeCellularDataPlanList(list);
}
break;  // There is only one connected or connecting cellular network.
}
}

available_devices_ = system->available_technologies;
enabled_devices_ = system->enabled_technologies;
connected_devices_ = system->connected_technologies;
offline_mode_ = system->offline_mode;

NotifyNetworkManagerChanged();
FreeSystemInfo(system);
}
