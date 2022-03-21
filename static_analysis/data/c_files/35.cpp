bool Plugin::LoadNaClModuleCommon(nacl::DescWrapper* wrapper,
NaClSubprocess* subprocess,
const Manifest* manifest,
bool should_report_uma,
ErrorInfo* error_info,
pp::CompletionCallback init_done_cb,
pp::CompletionCallback crash_cb) {
ServiceRuntime* new_service_runtime =
new ServiceRuntime(this, manifest, should_report_uma, init_done_cb,
crash_cb);
subprocess->set_service_runtime(new_service_runtime);
PLUGIN_PRINTF(("Plugin::LoadNaClModuleCommon (service_runtime=%p)\n",
static_cast<void*>(new_service_runtime)));
if (NULL == new_service_runtime) {
error_info->SetReport(ERROR_SEL_LDR_INIT,
"sel_ldr init failure " + subprocess->description());
return false;
}

bool service_runtime_started =
      new_service_runtime->Start(wrapper,
                                 error_info,
                                 manifest_base_url());
PLUGIN_PRINTF(("Plugin::LoadNaClModuleCommon (service_runtime_started=%d)\n",
service_runtime_started));
if (!service_runtime_started) {
return false;
}
  // Try to start the Chrome IPC-based proxy.
  const PPB_NaCl_Private* ppb_nacl = GetNaclInterface();
  if (ppb_nacl->StartPpapiProxy(pp_instance())) {
    using_ipc_proxy_ = true;
    // We need to explicitly schedule this here. It is normally called in
    // response to starting the SRPC proxy.
    CHECK(init_done_cb.pp_completion_callback().func != NULL);
    PLUGIN_PRINTF(("Plugin::LoadNaClModuleCommon, started ipc proxy.\n"));
    pp::Module::Get()->core()->CallOnMainThread(0, init_done_cb, PP_OK);
  }
return true;
}
