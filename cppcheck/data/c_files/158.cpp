iakerb_gss_delete_sec_context(OM_uint32 *minor_status,
gss_ctx_id_t *context_handle,
gss_buffer_t output_token)
{
    OM_uint32 major_status = GSS_S_COMPLETE;

if (output_token != GSS_C_NO_BUFFER) {
output_token->length = 0;
output_token->value = NULL;
}

*minor_status = 0;

    if (*context_handle != GSS_C_NO_CONTEXT) {
        iakerb_ctx_id_t iakerb_ctx = (iakerb_ctx_id_t)*context_handle;
        if (iakerb_ctx->magic == KG_IAKERB_CONTEXT) {
            iakerb_release_context(iakerb_ctx);
            *context_handle = GSS_C_NO_CONTEXT;
        } else {
            assert(iakerb_ctx->magic == KG_CONTEXT);
            major_status = krb5_gss_delete_sec_context(minor_status,
                                                       context_handle,
                                                       output_token);
        }
    }
    return major_status;
}
