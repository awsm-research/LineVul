int _our_safe_pcap_next_ex(pcap_t *pcap, struct pcap_pkthdr **pkthdr,
const u_char **pktdata, const char *funcname,
const int line, const char *file)
{
int res = pcap_next_ex(pcap, pkthdr, pktdata);

if (*pktdata && *pkthdr) {
if ((*pkthdr)->len > MAXPACKET) {
fprintf(stderr, "safe_pcap_next_ex ERROR: Invalid packet length in %s:%s() line %d: %u is greater than maximum %u\n",
file, funcname, line, (*pkthdr)->len, MAXPACKET);
exit(-1);
}

        if ((*pkthdr)->len < (*pkthdr)->caplen) {
            fprintf(stderr, "safe_pcap_next_ex ERROR: Invalid packet length in %s:%s() line %d: packet length %u is less than capture length %u\n",
file, funcname, line, (*pkthdr)->len, (*pkthdr)->caplen);
exit(-1);
}
}

return res;
}
