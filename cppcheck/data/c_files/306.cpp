krb5_gss_inquire_sec_context_by_oid (OM_uint32 *minor_status,
const gss_ctx_id_t context_handle,
const gss_OID desired_object,
gss_buffer_set_t *data_set)
{
krb5_gss_ctx_id_rec *ctx;
size_t i;

if (minor_status == NULL)
return GSS_S_CALL_INACCESSIBLE_WRITE;

*minor_status = 0;

if (desired_object == GSS_C_NO_OID)
return GSS_S_CALL_INACCESSIBLE_READ;

if (data_set == NULL)
return GSS_S_CALL_INACCESSIBLE_WRITE;

*data_set = GSS_C_NO_BUFFER_SET;

ctx = (krb5_gss_ctx_id_rec *) context_handle;

    if (!ctx->established)
return GSS_S_NO_CONTEXT;

for (i = 0; i < sizeof(krb5_gss_inquire_sec_context_by_oid_ops)/
sizeof(krb5_gss_inquire_sec_context_by_oid_ops[0]); i++) {
if (g_OID_prefix_equal(desired_object, &krb5_gss_inquire_sec_context_by_oid_ops[i].oid)) {
return (*krb5_gss_inquire_sec_context_by_oid_ops[i].func)(minor_status,
context_handle,
desired_object,
data_set);
}
}

*minor_status = EINVAL;

return GSS_S_UNAVAILABLE;
}
