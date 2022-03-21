void __ip_select_ident(struct iphdr *iph, int segs)
{
static u32 ip_idents_hashrnd __read_mostly;
u32 hash, id;

net_get_random_once(&ip_idents_hashrnd, sizeof(ip_idents_hashrnd));

hash = jhash_3words((__force u32)iph->daddr,
(__force u32)iph->saddr,
			    iph->protocol,
ip_idents_hashrnd);
id = ip_idents_reserve(hash, segs);
iph->id = htons(id);
}
