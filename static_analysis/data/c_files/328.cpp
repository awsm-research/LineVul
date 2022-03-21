get_caller_uid (GDBusMethodInvocation *context, gint *uid)
{
        PolkitSubject *subject;
        PolkitSubject *process;

        subject = polkit_system_bus_name_new (g_dbus_method_invocation_get_sender (context));
        process = polkit_system_bus_name_get_process_sync (POLKIT_SYSTEM_BUS_NAME (subject), NULL, NULL);
        if (!process) {
                g_object_unref (subject);
return FALSE;
}

        *uid = polkit_unix_process_get_uid (POLKIT_UNIX_PROCESS (process));
        g_object_unref (subject);
        g_object_unref (process);

return TRUE;
}
