int ip6_find_1stfragopt(struct sk_buff *skb, u8 **nexthdr)
{
u16 offset = sizeof(struct ipv6hdr);
	struct ipv6_opt_hdr *exthdr =
				(struct ipv6_opt_hdr *)(ipv6_hdr(skb) + 1);
unsigned int packet_len = skb_tail_pointer(skb) -
skb_network_header(skb);
int found_rhdr = 0;
*nexthdr = &ipv6_hdr(skb)->nexthdr;

	while (offset + 1 <= packet_len) {

switch (**nexthdr) {

case NEXTHDR_HOP:
break;
case NEXTHDR_ROUTING:
found_rhdr = 1;
break;
case NEXTHDR_DEST:
#if IS_ENABLED(CONFIG_IPV6_MIP6)
if (ipv6_find_tlv(skb, offset, IPV6_TLV_HAO) >= 0)
break;
#endif
if (found_rhdr)
return offset;
break;
default:
return offset;
}

		offset += ipv6_optlen(exthdr);
		*nexthdr = &exthdr->nexthdr;
exthdr = (struct ipv6_opt_hdr *)(skb_network_header(skb) +
offset);
}

	return offset;
}
