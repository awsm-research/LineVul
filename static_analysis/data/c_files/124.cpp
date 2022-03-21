void RegisterProperties(IBusPropList* ibus_prop_list) {
    DLOG(INFO) << "RegisterProperties" << (ibus_prop_list ? "" : " (clear)");

ImePropertyList prop_list;  // our representation.
if (ibus_prop_list) {
// You can call
//   LOG(INFO) << "\n" << PrintPropList(ibus_prop_list, 0);
// here to dump |ibus_prop_list|.
if (!FlattenPropertyList(ibus_prop_list, &prop_list)) {
// Clear properties on errors.
        RegisterProperties(NULL);
return;
}
}
// Notify the change.
    register_ime_properties_(language_library_, prop_list);
  }
