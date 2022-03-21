static int dccp_packet(struct nf_conn *ct, const struct sk_buff *skb,
unsigned int dataoff, enum ip_conntrack_info ctinfo,
u_int8_t pf, unsigned int hooknum,
unsigned int *timeouts)
{
struct net *net = nf_ct_net(ct);
enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
struct dccp_hdr _dh, *dh;
u_int8_t type, old_state, new_state;
enum ct_dccp_roles role;

	dh = skb_header_pointer(skb, dataoff, sizeof(_dh), &dh);
BUG_ON(dh == NULL);
type = dh->dccph_type;

if (type == DCCP_PKT_RESET &&
!test_bit(IPS_SEEN_REPLY_BIT, &ct->status)) {
/* Tear down connection immediately if only reply is a RESET */
nf_ct_kill_acct(ct, ctinfo, skb);
return NF_ACCEPT;
}

spin_lock_bh(&ct->lock);

role = ct->proto.dccp.role[dir];
old_state = ct->proto.dccp.state;
new_state = dccp_state_table[role][type][old_state];

switch (new_state) {
case CT_DCCP_REQUEST:
if (old_state == CT_DCCP_TIMEWAIT &&
role == CT_DCCP_ROLE_SERVER) {
/* Reincarnation in the reverse direction: reopen and
* reverse client/server roles. */
ct->proto.dccp.role[dir] = CT_DCCP_ROLE_CLIENT;
ct->proto.dccp.role[!dir] = CT_DCCP_ROLE_SERVER;
}
break;
case CT_DCCP_RESPOND:
if (old_state == CT_DCCP_REQUEST)
ct->proto.dccp.handshake_seq = dccp_hdr_seq(dh);
break;
case CT_DCCP_PARTOPEN:
if (old_state == CT_DCCP_RESPOND &&
type == DCCP_PKT_ACK &&
dccp_ack_seq(dh) == ct->proto.dccp.handshake_seq)
set_bit(IPS_ASSURED_BIT, &ct->status);
break;
case CT_DCCP_IGNORE:
/*
* Connection tracking might be out of sync, so we ignore
* packets that might establish a new connection and resync
* if the server responds with a valid Response.
*/
if (ct->proto.dccp.last_dir == !dir &&
ct->proto.dccp.last_pkt == DCCP_PKT_REQUEST &&
type == DCCP_PKT_RESPONSE) {
ct->proto.dccp.role[!dir] = CT_DCCP_ROLE_CLIENT;
ct->proto.dccp.role[dir] = CT_DCCP_ROLE_SERVER;
ct->proto.dccp.handshake_seq = dccp_hdr_seq(dh);
new_state = CT_DCCP_RESPOND;
break;
}
ct->proto.dccp.last_dir = dir;
ct->proto.dccp.last_pkt = type;

spin_unlock_bh(&ct->lock);
if (LOG_INVALID(net, IPPROTO_DCCP))
nf_log_packet(net, pf, 0, skb, NULL, NULL, NULL,
"nf_ct_dccp: invalid packet ignored ");
return NF_ACCEPT;
case CT_DCCP_INVALID:
spin_unlock_bh(&ct->lock);
if (LOG_INVALID(net, IPPROTO_DCCP))
nf_log_packet(net, pf, 0, skb, NULL, NULL, NULL,
"nf_ct_dccp: invalid state transition ");
return -NF_ACCEPT;
}

ct->proto.dccp.last_dir = dir;
ct->proto.dccp.last_pkt = type;
ct->proto.dccp.state = new_state;
spin_unlock_bh(&ct->lock);

if (new_state != old_state)
nf_conntrack_event_cache(IPCT_PROTOINFO, ct);

nf_ct_refresh_acct(ct, ctinfo, skb, timeouts[new_state]);

return NF_ACCEPT;
}
