kdc_process_for_user(kdc_realm_t *kdc_active_realm,
krb5_pa_data *pa_data,
krb5_keyblock *tgs_session,
krb5_pa_s4u_x509_user **s4u_x509_user,
const char **status)
{
krb5_error_code             code;
krb5_pa_for_user            *for_user;
krb5_data                   req_data;

req_data.length = pa_data->length;
req_data.data = (char *)pa_data->contents;

code = decode_krb5_pa_for_user(&req_data, &for_user);
    if (code)
return code;

code = verify_for_user_checksum(kdc_context, tgs_session, for_user);
if (code) {
*status = "INVALID_S4U2SELF_CHECKSUM";
krb5_free_pa_for_user(kdc_context, for_user);
return code;
}

*s4u_x509_user = calloc(1, sizeof(krb5_pa_s4u_x509_user));
if (*s4u_x509_user == NULL) {
krb5_free_pa_for_user(kdc_context, for_user);
return ENOMEM;
}

(*s4u_x509_user)->user_id.user = for_user->user;
for_user->user = NULL;
krb5_free_pa_for_user(kdc_context, for_user);

return 0;
}
