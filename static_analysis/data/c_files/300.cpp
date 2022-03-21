rx_cache_insert(netdissect_options *ndo,
const u_char *bp, const struct ip *ip, int dport)
{
struct rx_cache_entry *rxent;
const struct rx_header *rxh = (const struct rx_header *) bp;

if (ndo->ndo_snapend - bp + 1 <= (int)(sizeof(struct rx_header) + sizeof(int32_t)))
return;

rxent = &rx_cache[rx_cache_next];

if (++rx_cache_next >= RX_CACHE_SIZE)
rx_cache_next = 0;

rxent->callnum = EXTRACT_32BITS(&rxh->callNumber);
UNALIGNED_MEMCPY(&rxent->client, &ip->ip_src, sizeof(uint32_t));
UNALIGNED_MEMCPY(&rxent->server, &ip->ip_dst, sizeof(uint32_t));
rxent->dport = dport;
	rxent->serviceId = EXTRACT_32BITS(&rxh->serviceId);
rxent->opcode = EXTRACT_32BITS(bp + sizeof(struct rx_header));
}
