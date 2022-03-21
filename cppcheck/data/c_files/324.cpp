l2tp_avp_print(netdissect_options *ndo, const u_char *dat, int length)
{
u_int len;
const uint16_t *ptr = (const uint16_t *)dat;
uint16_t attr_type;
int hidden = FALSE;

if (length <= 0) {
return;
}

ND_PRINT((ndo, " "));

ND_TCHECK(*ptr);	/* Flags & Length */
len = EXTRACT_16BITS(ptr) & L2TP_AVP_HDR_LEN_MASK;

/* If it is not long enough to contain the header, we'll give up. */
if (len < 6)
goto trunc;

/* If it goes past the end of the remaining length of the packet,
we'll give up. */
if (len > (u_int)length)
goto trunc;

/* If it goes past the end of the remaining length of the captured
data, we'll give up. */
ND_TCHECK2(*ptr, len);
	/* After this point, no need to worry about truncation *

if (EXTRACT_16BITS(ptr) & L2TP_AVP_HDR_FLAG_MANDATORY) {
ND_PRINT((ndo, "*"));
}
if (EXTRACT_16BITS(ptr) & L2TP_AVP_HDR_FLAG_HIDDEN) {
hidden = TRUE;
ND_PRINT((ndo, "?"));
}
ptr++;

if (EXTRACT_16BITS(ptr)) {
/* Vendor Specific Attribute */
ND_PRINT((ndo, "VENDOR%04x:", EXTRACT_16BITS(ptr))); ptr++;
ND_PRINT((ndo, "ATTR%04x", EXTRACT_16BITS(ptr))); ptr++;
ND_PRINT((ndo, "("));
print_octets(ndo, (const u_char *)ptr, len-6);
ND_PRINT((ndo, ")"));
} else {
/* IETF-defined Attributes */
ptr++;
attr_type = EXTRACT_16BITS(ptr); ptr++;
ND_PRINT((ndo, "%s", tok2str(l2tp_avp2str, "AVP-#%u", attr_type)));
ND_PRINT((ndo, "("));
if (hidden) {
ND_PRINT((ndo, "???"));
} else {
switch (attr_type) {
case L2TP_AVP_MSGTYPE:
				l2tp_msgtype_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_RESULT_CODE:
l2tp_result_code_print(ndo, (const u_char *)ptr, len-6);
break;
case L2TP_AVP_PROTO_VER:
				l2tp_proto_ver_print(ndo, ptr);
break;
case L2TP_AVP_FRAMING_CAP:
				l2tp_framing_cap_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_BEARER_CAP:
				l2tp_bearer_cap_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_TIE_BREAKER:
print_octets(ndo, (const u_char *)ptr, 8);
break;
case L2TP_AVP_FIRM_VER:
case L2TP_AVP_ASSND_TUN_ID:
case L2TP_AVP_RECV_WIN_SIZE:
case L2TP_AVP_ASSND_SESS_ID:
print_16bits_val(ndo, ptr);
break;
case L2TP_AVP_HOST_NAME:
case L2TP_AVP_VENDOR_NAME:
case L2TP_AVP_CALLING_NUMBER:
case L2TP_AVP_CALLED_NUMBER:
case L2TP_AVP_SUB_ADDRESS:
case L2TP_AVP_PROXY_AUTH_NAME:
case L2TP_AVP_PRIVATE_GRP_ID:
print_string(ndo, (const u_char *)ptr, len-6);
break;
case L2TP_AVP_CHALLENGE:
case L2TP_AVP_INI_RECV_LCP:
case L2TP_AVP_LAST_SENT_LCP:
case L2TP_AVP_LAST_RECV_LCP:
case L2TP_AVP_PROXY_AUTH_CHAL:
case L2TP_AVP_PROXY_AUTH_RESP:
case L2TP_AVP_RANDOM_VECTOR:
print_octets(ndo, (const u_char *)ptr, len-6);
break;
case L2TP_AVP_Q931_CC:
l2tp_q931_cc_print(ndo, (const u_char *)ptr, len-6);
break;
case L2TP_AVP_CHALLENGE_RESP:
print_octets(ndo, (const u_char *)ptr, 16);
break;
case L2TP_AVP_CALL_SER_NUM:
case L2TP_AVP_MINIMUM_BPS:
case L2TP_AVP_MAXIMUM_BPS:
case L2TP_AVP_TX_CONN_SPEED:
case L2TP_AVP_PHY_CHANNEL_ID:
case L2TP_AVP_RX_CONN_SPEED:
print_32bits_val(ndo, (const uint32_t *)ptr);
break;
case L2TP_AVP_BEARER_TYPE:
				l2tp_bearer_type_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_FRAMING_TYPE:
				l2tp_framing_type_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_PACKET_PROC_DELAY:
l2tp_packet_proc_delay_print(ndo);
break;
case L2TP_AVP_PROXY_AUTH_TYPE:
				l2tp_proxy_auth_type_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_PROXY_AUTH_ID:
				l2tp_proxy_auth_id_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_CALL_ERRORS:
				l2tp_call_errors_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_ACCM:
				l2tp_accm_print(ndo, (const u_char *)ptr);
break;
case L2TP_AVP_SEQ_REQUIRED:
break;	/* No Attribute Value */
case L2TP_AVP_PPP_DISCON_CC:
l2tp_ppp_discon_cc_print(ndo, (const u_char *)ptr, len-6);
break;
default:
break;
}
}
ND_PRINT((ndo, ")"));
}

l2tp_avp_print(ndo, dat+len, length-len);
return;

trunc:
ND_PRINT((ndo, "|..."));
}
