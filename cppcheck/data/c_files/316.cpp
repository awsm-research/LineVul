cib_remote_dispatch(gpointer user_data)
{
cib_t *cib = user_data;
cib_remote_opaque_t *private = cib->variant_opaque;

xmlNode *msg = NULL;
    const char *type = NULL;

crm_info("Message on callback channel");
    msg = crm_recv_remote_msg(private->callback.session, private->callback.encrypted);

    type = crm_element_value(msg, F_TYPE);
    crm_trace("Activating %s callbacks...", type);

    if (safe_str_eq(type, T_CIB)) {
        cib_native_callback(cib, msg, 0, 0);

    } else if (safe_str_eq(type, T_CIB_NOTIFY)) {
        g_list_foreach(cib->notify_list, cib_native_notify, msg);

    } else {
        crm_err("Unknown message type: %s", type);
    }

    if (msg != NULL) {
free_xml(msg);
        return 0;
}
    return -1;
}
