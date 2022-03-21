static int udp_v6_push_pending_frames(struct sock *sk)
{
struct sk_buff *skb;
struct udphdr *uh;
struct udp_sock  *up = udp_sk(sk);
struct inet_sock *inet = inet_sk(sk);
	struct flowi6 *fl6 = &inet->cork.fl.u.ip6;
int err = 0;
int is_udplite = IS_UDPLITE(sk);
__wsum csum = 0;

/* Grab the skbuff where UDP header space exists. */
if ((skb = skb_peek(&sk->sk_write_queue)) == NULL)
goto out;

/*
* Create a UDP header
*/
uh = udp_hdr(skb);
uh->source = fl6->fl6_sport;
uh->dest = fl6->fl6_dport;
uh->len = htons(up->len);
uh->check = 0;

if (is_udplite)
csum = udplite_csum_outgoing(sk, skb);
else if (skb->ip_summed == CHECKSUM_PARTIAL) { /* UDP hardware csum */
udp6_hwcsum_outgoing(sk, skb, &fl6->saddr, &fl6->daddr,
up->len);
goto send;
} else
csum = udp_csum_outgoing(sk, skb);

/* add protocol-dependent pseudo-header */
uh->check = csum_ipv6_magic(&fl6->saddr, &fl6->daddr,
up->len, fl6->flowi6_proto, csum);
if (uh->check == 0)
uh->check = CSUM_MANGLED_0;

send:
err = ip6_push_pending_frames(sk);
if (err) {
if (err == -ENOBUFS && !inet6_sk(sk)->recverr) {
UDP6_INC_STATS_USER(sock_net(sk),
UDP_MIB_SNDBUFERRORS, is_udplite);
err = 0;
}
} else
UDP6_INC_STATS_USER(sock_net(sk),
UDP_MIB_OUTDATAGRAMS, is_udplite);
out:
up->len = 0;
up->pending = 0;
return err;
}
