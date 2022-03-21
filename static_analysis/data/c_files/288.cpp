rename_principal_2_svc(rprinc_arg *arg, struct svc_req *rqstp)
{
static generic_ret          ret;
    char                        *prime_arg1,
        *prime_arg2;
    gss_buffer_desc             client_name,
        service_name;
OM_uint32                   minor_stat;
kadm5_server_handle_t       handle;
restriction_t               *rp;
const char                  *errmsg = NULL;
size_t                      tlen1, tlen2, clen, slen;
char                        *tdots1, *tdots2, *cdots, *sdots;

xdr_free(xdr_generic_ret, &ret);

if ((ret.code = new_server_handle(arg->api_version, rqstp, &handle)))
goto exit_func;

if ((ret.code = check_handle((void *)handle)))
goto exit_func;

if (setup_gss_names(rqstp, &client_name, &service_name) < 0) {
ret.code = KADM5_FAILURE;
goto exit_func;
}
if (krb5_unparse_name(handle->context, arg->src, &prime_arg1) ||
krb5_unparse_name(handle->context, arg->dest, &prime_arg2)) {
ret.code = KADM5_BAD_PRINCIPAL;
goto exit_func;
}
tlen1 = strlen(prime_arg1);
trunc_name(&tlen1, &tdots1);
tlen2 = strlen(prime_arg2);
trunc_name(&tlen2, &tdots2);
clen = client_name.length;
trunc_name(&clen, &cdots);
slen = service_name.length;
trunc_name(&slen, &sdots);

ret.code = KADM5_OK;
if (! CHANGEPW_SERVICE(rqstp)) {
if (!kadm5int_acl_check(handle->context, rqst2name(rqstp),
ACL_DELETE, arg->src, NULL))
ret.code = KADM5_AUTH_DELETE;
/* any restrictions at all on the ADD kills the RENAME */
if (!kadm5int_acl_check(handle->context, rqst2name(rqstp),
ACL_ADD, arg->dest, &rp) || rp) {
if (ret.code == KADM5_AUTH_DELETE)
ret.code = KADM5_AUTH_INSUFFICIENT;
else
ret.code = KADM5_AUTH_ADD;
}
} else
ret.code = KADM5_AUTH_INSUFFICIENT;
if (ret.code != KADM5_OK) {
/* okay to cast lengths to int because trunc_name limits max value */
krb5_klog_syslog(LOG_NOTICE,
_("Unauthorized request: kadm5_rename_principal, "
"%.*s%s to %.*s%s, "
"client=%.*s%s, service=%.*s%s, addr=%s"),
(int)tlen1, prime_arg1, tdots1,
(int)tlen2, prime_arg2, tdots2,
(int)clen, (char *)client_name.value, cdots,
(int)slen, (char *)service_name.value, sdots,
client_addr(rqstp->rq_xprt));
} else {
ret.code = kadm5_rename_principal((void *)handle, arg->src,
arg->dest);
if( ret.code != 0 )
errmsg = krb5_get_error_message(handle->context, ret.code);

/* okay to cast lengths to int because trunc_name limits max value */
krb5_klog_syslog(LOG_NOTICE,
_("Request: kadm5_rename_principal, "
"%.*s%s to %.*s%s, %s, "
"client=%.*s%s, service=%.*s%s, addr=%s"),
(int)tlen1, prime_arg1, tdots1,
(int)tlen2, prime_arg2, tdots2,
errmsg ? errmsg : _("success"),
(int)clen, (char *)client_name.value, cdots,
(int)slen, (char *)service_name.value, sdots,
client_addr(rqstp->rq_xprt));

if (errmsg != NULL)
krb5_free_error_message(handle->context, errmsg);

}
free(prime_arg1);
free(prime_arg2);
gss_release_buffer(&minor_stat, &client_name);
gss_release_buffer(&minor_stat, &service_name);
exit_func:
free_server_handle(handle);
return &ret;
}
