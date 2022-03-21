int ssl3_send_alert(SSL *s, int level, int desc)
{
/* Map tls/ssl alert value to correct one */
desc = s->method->ssl3_enc->alert_value(desc);
if (s->version == SSL3_VERSION && desc == SSL_AD_PROTOCOL_VERSION)
desc = SSL_AD_HANDSHAKE_FAILURE; /* SSL 3.0 does not have
* protocol_version alerts */
* protocol_version alerts */
if (desc < 0)
return -1;
    /* If a fatal one, remove from cache *
    if ((level == 2) && (s->session != NULL))
        SSL_CTX_remove_session(s->session_ctx, s->session);

s->s3->alert_dispatch = 1;
s->s3->send_alert[0] = level;
* else data is still being written out, we will get written some time in
* the future
*/
return -1;
}
