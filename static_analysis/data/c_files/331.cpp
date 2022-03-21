ikev2_auth_print(netdissect_options *ndo, u_char tpay,
const struct isakmp_gen *ext,
u_int item_len _U_, const u_char *ep,
uint32_t phase _U_, uint32_t doi _U_,
uint32_t proto _U_, int depth _U_)
{
struct ikev2_auth a;
const char *v2_auth[]={ "invalid", "rsasig",
"shared-secret", "dsssig" };
const u_char *authdata = (const u_char*)ext + sizeof(a);
unsigned int len;

	ND_TCHECK(*ext);
UNALIGNED_MEMCPY(&a, ext, sizeof(a));
ikev2_pay_print(ndo, NPSTR(tpay), a.h.critical);
len = ntohs(a.h.len);

/*
* Our caller has ensured that the length is >= 4.
*/
ND_PRINT((ndo," len=%u method=%s", len-4,
STR_OR_ID(a.auth_method, v2_auth)));
if (len > 4) {
if (ndo->ndo_vflag > 1) {
ND_PRINT((ndo, " authdata=("));
if (!rawprint(ndo, (const uint8_t *)authdata, len - sizeof(a)))
goto trunc;
ND_PRINT((ndo, ") "));
} else if (ndo->ndo_vflag) {
if (!ike_show_somedata(ndo, authdata, ep))
goto trunc;
}
}

return (const u_char *)ext + len;
trunc:
ND_PRINT((ndo," [|%s]", NPSTR(tpay)));
return NULL;
}
