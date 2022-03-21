frag6_print(netdissect_options *ndo, register const u_char *bp, register const u_char *bp2)
{
register const struct ip6_frag *dp;
register const struct ip6_hdr *ip6;

dp = (const struct ip6_frag *)bp;
ip6 = (const struct ip6_hdr *)bp2;

	ND_TCHECK(dp->ip6f_offlg);

if (ndo->ndo_vflag) {
ND_PRINT((ndo, "frag (0x%08x:%d|%ld)",
EXTRACT_32BITS(&dp->ip6f_ident),
EXTRACT_16BITS(&dp->ip6f_offlg) & IP6F_OFF_MASK,
sizeof(struct ip6_hdr) + EXTRACT_16BITS(&ip6->ip6_plen) -
(long)(bp - bp2) - sizeof(struct ip6_frag)));
} else {
ND_PRINT((ndo, "frag (%d|%ld)",
EXTRACT_16BITS(&dp->ip6f_offlg) & IP6F_OFF_MASK,
sizeof(struct ip6_hdr) + EXTRACT_16BITS(&ip6->ip6_plen) -
(long)(bp - bp2) - sizeof(struct ip6_frag)));
}

/* it is meaningless to decode non-first fragment */
if ((EXTRACT_16BITS(&dp->ip6f_offlg) & IP6F_OFF_MASK) != 0)
return -1;
else
{
ND_PRINT((ndo, " "));
return sizeof(struct ip6_frag);
}
trunc:
ND_PRINT((ndo, "[|frag]"));
return -1;
}
