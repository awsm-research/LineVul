u32 secure_ipv4_port_ephemeral(__be32 saddr, __be32 daddr, __be16 dport)
{
	struct keydata *keyptr = get_keyptr();
	u32 hash[4];
	/*
	 *  Pick a unique starting offset for each ephemeral port search
	 *  (saddr, daddr, dport) and 48bits of random data.
	 *
	hash[0] = (__force u32)saddr;
	hash[1] = (__force u32)daddr;
	hash[2] = (__force u32)dport ^ keyptr->secret[10];
	hash[3] = keyptr->secret[11];
	return half_md4_transform(hash, keyptr->secret);
}
