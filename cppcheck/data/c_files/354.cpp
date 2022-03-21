spnego_gss_wrap_iov(OM_uint32 *minor_status,
gss_ctx_id_t context_handle,
int conf_req_flag,
gss_qop_t qop_req,
int *conf_state,
gss_iov_buffer_desc *iov,
int iov_count)
{
OM_uint32 ret;
ret = gss_wrap_iov(minor_status,
			   context_handle,
conf_req_flag,
qop_req,
conf_state,
iov,
iov_count);
return (ret);
}
