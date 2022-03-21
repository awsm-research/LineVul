atm_if_print(netdissect_options *ndo,
const struct pcap_pkthdr *h, const u_char *p)
{
u_int caplen = h->caplen;
u_int length = h->len;
uint32_t llchdr;
u_int hdrlen = 0;

if (caplen < 1 || length < 1) {
ND_PRINT((ndo, "%s", tstr));
return (caplen);
}

/* Cisco Style NLPID ? */
if (*p == LLC_UI) {
if (ndo->ndo_eflag)
ND_PRINT((ndo, "CNLPID "));
            isoclns_print(ndo, p + 1, length - 1, caplen - 1);
return hdrlen;
}

/*
* Must have at least a DSAP, an SSAP, and the first byte of the
* control field.
*/
if (caplen < 3 || length < 3) {
ND_PRINT((ndo, "%s", tstr));
return (caplen);
}

/*
* Extract the presumed LLC header into a variable, for quick
* testing.
* Then check for a header that's neither a header for a SNAP
* packet nor an RFC 2684 routed NLPID-formatted PDU nor
* an 802.2-but-no-SNAP IP packet.
*/
llchdr = EXTRACT_24BITS(p);
if (llchdr != LLC_UI_HDR(LLCSAP_SNAP) &&
llchdr != LLC_UI_HDR(LLCSAP_ISONS) &&
llchdr != LLC_UI_HDR(LLCSAP_IP)) {
/*
* XXX - assume 802.6 MAC header from Fore driver.
*
* Unfortunately, the above list doesn't check for
* all known SAPs, doesn't check for headers where
* the source and destination SAP aren't the same,
* and doesn't check for non-UI frames.  It also
* runs the risk of an 802.6 MAC header that happens
* to begin with one of those values being
* incorrectly treated as an 802.2 header.
*
* So is that Fore driver still around?  And, if so,
* is it still putting 802.6 MAC headers on ATM
* packets?  If so, could it be changed to use a
* new DLT_IEEE802_6 value if we added it?
*/
if (caplen < 20 || length < 20) {
ND_PRINT((ndo, "%s", tstr));
return (caplen);
}
if (ndo->ndo_eflag)
ND_PRINT((ndo, "%08x%08x %08x%08x ",
EXTRACT_32BITS(p),
EXTRACT_32BITS(p+4),
EXTRACT_32BITS(p+8),
EXTRACT_32BITS(p+12)));
p += 20;
length -= 20;
caplen -= 20;
hdrlen += 20;
}
hdrlen += atm_llc_print(ndo, p, length, caplen);
return (hdrlen);
}
