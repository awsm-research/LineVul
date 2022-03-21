static void build_l4proto_sctp(const struct nf_conntrack *ct, struct nethdr *n)
{
	ct_build_group(ct, ATTR_GRP_ORIG_PORT, n, NTA_PORT,
		      sizeof(struct nfct_attr_grp_port));
if (!nfct_attr_is_set(ct, ATTR_SCTP_STATE))
return;

ct_build_u8(ct, ATTR_SCTP_STATE, n, NTA_SCTP_STATE);
ct_build_u32(ct, ATTR_SCTP_VTAG_ORIG, n, NTA_SCTP_VTAG_ORIG);
ct_build_u32(ct, ATTR_SCTP_VTAG_REPL, n, NTA_SCTP_VTAG_REPL);
}
