static inline int ip6_ufo_append_data(struct sock *sk,
int getfrag(void *from, char *to, int offset, int len,
int odd, struct sk_buff *skb),
void *from, int length, int hh_len, int fragheaderlen,
int transhdrlen, int mtu,unsigned int flags,
struct rt6_info *rt)

{
struct sk_buff *skb;
int err;

/* There is support for UDP large send offload by network
* device, so create one single skb packet containing complete
* udp datagram
*/
if ((skb = skb_peek_tail(&sk->sk_write_queue)) == NULL) {
skb = sock_alloc_send_skb(sk,
hh_len + fragheaderlen + transhdrlen + 20,
(flags & MSG_DONTWAIT), &err);
if (skb == NULL)
return err;

/* reserve space for Hardware header */
skb_reserve(skb, hh_len);

/* create space for UDP/IP header */
skb_put(skb,fragheaderlen + transhdrlen);

/* initialize network header pointer */
skb_reset_network_header(skb);

/* initialize protocol header pointer */
skb->transport_header = skb->network_header + fragheaderlen;

skb->protocol = htons(ETH_P_IPV6);
skb->ip_summed = CHECKSUM_PARTIAL;
skb->csum = 0;
	}
	err = skb_append_datato_frags(sk,skb, getfrag, from,
				      (length - transhdrlen));
	if (!err) {
		struct frag_hdr fhdr;

/* Specify the length of each IPv6 datagram fragment.
* It has to be a multiple of 8.
*/
skb_shinfo(skb)->gso_size = (mtu - fragheaderlen -
sizeof(struct frag_hdr)) & ~7;
skb_shinfo(skb)->gso_type = SKB_GSO_UDP;
ipv6_select_ident(&fhdr, rt);
skb_shinfo(skb)->ip6_frag_id = fhdr.identification;
__skb_queue_tail(&sk->sk_write_queue, skb);
		return 0;
}
	/* There is not enough support do UPD LSO,
	 * so follow normal path
	 *
	kfree_skb(skb);

	return err;
}
