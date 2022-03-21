static void UpdatePropertyCallback(IBusPanelService* panel,
                                     IBusProperty* ibus_prop,
                                     gpointer user_data) {
    g_return_if_fail(user_data);
    InputMethodStatusConnection* self
        = static_cast<InputMethodStatusConnection*>(user_data);
    self->UpdateProperty(ibus_prop);
}
