sctp_disposition_t sctp_sf_ootb(struct net *net,
const struct sctp_endpoint *ep,
const struct sctp_association *asoc,
const sctp_subtype_t type,
void *arg,
sctp_cmd_seq_t *commands)
{
struct sctp_chunk *chunk = arg;
struct sk_buff *skb = chunk->skb;
sctp_chunkhdr_t *ch;
sctp_errhdr_t *err;
__u8 *ch_end;
int ootb_shut_ack = 0;
int ootb_cookie_ack = 0;

SCTP_INC_STATS(net, SCTP_MIB_OUTOFBLUES);

ch = (sctp_chunkhdr_t *) chunk->chunk_hdr;
do {
/* Report violation if the chunk is less then minimal */
if (ntohs(ch->length) < sizeof(sctp_chunkhdr_t))
return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
commands);

/* Now that we know we at least have a chunk header,
* do things that are type appropriate.
*/
if (SCTP_CID_SHUTDOWN_ACK == ch->type)
ootb_shut_ack = 1;

/* RFC 2960, Section 3.3.7
*   Moreover, under any circumstances, an endpoint that
*   receives an ABORT  MUST NOT respond to that ABORT by
*   sending an ABORT of its own.
*/
if (SCTP_CID_ABORT == ch->type)
return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

/* RFC 8.4, 7) If the packet contains a "Stale cookie" ERROR
* or a COOKIE ACK the SCTP Packet should be silently
* discarded.
*/

if (SCTP_CID_COOKIE_ACK == ch->type)
ootb_cookie_ack = 1;

if (SCTP_CID_ERROR == ch->type) {
sctp_walk_errors(err, ch) {
if (SCTP_ERROR_STALE_COOKIE == err->cause) {
ootb_cookie_ack = 1;
break;
}
}
}

		/* Report violation if chunk len overflows *
		ch_end = ((__u8 *)ch) + SCTP_PAD4(ntohs(ch->length));
		if (ch_end > skb_tail_pointer(skb))
			return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
						  commands);
ch = (sctp_chunkhdr_t *) ch_end;
} while (ch_end < skb_tail_pointer(skb));

if (ootb_shut_ack)
return sctp_sf_shut_8_4_5(net, ep, asoc, type, arg, commands);
else if (ootb_cookie_ack)
return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
else
return sctp_sf_tabort_8_4_8(net, ep, asoc, type, arg, commands);
}
