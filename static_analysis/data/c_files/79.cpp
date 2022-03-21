accept_ice_connection (GIOChannel           *source,
GIOCondition          condition,
GsmIceConnectionData *data)
{
        IceListenObj    listener;
IceConn         ice_conn;
IceAcceptStatus status;
        GsmClient      *client;
        GsmXsmpServer  *server;
        listener = data->listener;
        server = data->server;

g_debug ("GsmXsmpServer: accept_ice_connection()");

        ice_conn = IceAcceptConnection (listener, &status);
if (status != IceAcceptSuccess) {
g_debug ("GsmXsmpServer: IceAcceptConnection returned %d", status);
return TRUE;
}

        client = gsm_xsmp_client_new (ice_conn);
        ice_conn->context = client;
        gsm_store_add (server->priv->client_store, gsm_client_peek_id (client), G_OBJECT (client));
        /* the store will own the ref *
        g_object_unref (client);

return TRUE;
}
