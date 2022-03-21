int DecodeTeredo(ThreadVars *tv, DecodeThreadVars *dtv, Packet *p, uint8_t *pkt, uint16_t len, PacketQueue *pq)
{
if (!g_teredo_enabled)
return TM_ECODE_FAILED;

uint8_t *start = pkt;

/* Is this packet to short to contain an IPv6 packet ? */
if (len < IPV6_HEADER_LEN)
return TM_ECODE_FAILED;

/* Teredo encapsulate IPv6 in UDP and can add some custom message
* part before the IPv6 packet. In our case, we just want to get
* over an ORIGIN indication. So we just make one offset if needed. */
if (start[0] == 0x0) {
switch (start[1]) {
/* origin indication: compatible with tunnel */
case 0x0:
/* offset is coherent with len and presence of an IPv6 header */
if (len >= TEREDO_ORIG_INDICATION_LENGTH + IPV6_HEADER_LEN)
start += TEREDO_ORIG_INDICATION_LENGTH;
else
return TM_ECODE_FAILED;
break;
/* authentication: negotiation not real tunnel */
case 0x1:
return TM_ECODE_FAILED;
/* this case is not possible in Teredo: not that protocol */
default:
return TM_ECODE_FAILED;
}
}

/* There is no specific field that we can check to prove that the packet
* is a Teredo packet. We've zapped here all the possible Teredo header
* and we should have an IPv6 packet at the start pointer.
     * We then can only do two checks before sending the encapsulated packets
* to decoding:
*  - The packet has a protocol version which is IPv6.
*  - The IPv6 length of the packet matches what remains in buffer.
*/
if (IP_GET_RAW_VER(start) == 6) {
IPV6Hdr *thdr = (IPV6Hdr *)start;
if (len ==  IPV6_HEADER_LEN +
IPV6_GET_RAW_PLEN(thdr) + (start - pkt)) {
if (pq != NULL) {
int blen = len - (start - pkt);
/* spawn off tunnel packet */
Packet *tp = PacketTunnelPktSetup(tv, dtv, p, start, blen,
                                                  DECODE_TUNNEL_IPV6, pq);
if (tp != NULL) {
PKT_SET_SRC(tp, PKT_SRC_DECODER_TEREDO);
/* add the tp to the packet queue. */
PacketEnqueue(pq,tp);
StatsIncr(tv, dtv->counter_teredo);
return TM_ECODE_OK;
}
}
}
return TM_ECODE_FAILED;
}

return TM_ECODE_FAILED;
}
