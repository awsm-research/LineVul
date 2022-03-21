acc_ctx_cont(OM_uint32 *minstat,
gss_buffer_t buf,
gss_ctx_id_t *ctx,
gss_buffer_t *responseToken,
gss_buffer_t *mechListMIC,
OM_uint32 *negState,
send_token_flag *return_token)
{
OM_uint32 ret, tmpmin;
gss_OID supportedMech;
spnego_gss_ctx_id_t sc;
unsigned int len;
unsigned char *ptr, *bufstart;

sc = (spnego_gss_ctx_id_t)*ctx;
ret = GSS_S_DEFECTIVE_TOKEN;
*negState = REJECT;
*minstat = 0;
supportedMech = GSS_C_NO_OID;
*return_token = ERROR_TOKEN_SEND;
*responseToken = *mechListMIC = GSS_C_NO_BUFFER;

ptr = bufstart = buf->value;
#define REMAIN (buf->length - (ptr - bufstart))
	if (REMAIN > INT_MAX)
return GSS_S_DEFECTIVE_TOKEN;

/*
* Attempt to work with old Sun SPNEGO.
*/
if (*ptr == HEADER_ID) {
ret = g_verify_token_header(gss_mech_spnego,
&len, &ptr, 0, REMAIN);
if (ret) {
*minstat = ret;
return GSS_S_DEFECTIVE_TOKEN;
}
}
if (*ptr != (CONTEXT | 0x01)) {
return GSS_S_DEFECTIVE_TOKEN;
}
ret = get_negTokenResp(minstat, ptr, REMAIN,
negState, &supportedMech,
responseToken, mechListMIC);
if (ret != GSS_S_COMPLETE)
goto cleanup;

if (*responseToken == GSS_C_NO_BUFFER &&
*mechListMIC == GSS_C_NO_BUFFER) {

ret = GSS_S_DEFECTIVE_TOKEN;
goto cleanup;
}
if (supportedMech != GSS_C_NO_OID) {
ret = GSS_S_DEFECTIVE_TOKEN;
goto cleanup;
}
sc->firstpass = 0;
*negState = ACCEPT_INCOMPLETE;
*return_token = CONT_TOKEN_SEND;
cleanup:
if (supportedMech != GSS_C_NO_OID) {
generic_gss_release_oid(&tmpmin, &supportedMech);
}
return ret;
#undef REMAIN
}
