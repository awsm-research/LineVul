on_unregister_handler(TCMUService1HandlerManager1 *interface,
GDBusMethodInvocation *invocation,
gchar *subtype,
gpointer user_data)
{
struct tcmur_handler *handler = find_handler_by_subtype(subtype);
struct dbus_info *info = handler ? handler->opaque : NULL;

if (!handler) {
g_dbus_method_invocation_return_value(invocation,
g_variant_new("(bs)", FALSE,
"unknown subtype"));
return TRUE;
}
dbus_unexport_handler(handler);
	tcmur_unregister_handler(handler);
g_bus_unwatch_name(info->watcher_id);
g_free(info);
g_free(handler);
g_dbus_method_invocation_return_value(invocation,
g_variant_new("(bs)", TRUE, "succeeded"));
return TRUE;
}
