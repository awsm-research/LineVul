mm_sshpam_init_ctx(Authctxt *authctxt)
{
Buffer m;
int success;

debug3("%s", __func__);
buffer_init(&m);
	buffer_put_cstring(&m, authctxt->user);
mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PAM_INIT_CTX, &m);
debug3("%s: waiting for MONITOR_ANS_PAM_INIT_CTX", __func__);
mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_PAM_INIT_CTX, &m);
success = buffer_get_int(&m);
if (success == 0) {
debug3("%s: pam_init_ctx failed", __func__);
buffer_free(&m);
return (NULL);
}
buffer_free(&m);
return (authctxt);
}
