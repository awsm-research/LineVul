int udp_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
size_t len, int noblock, int flags, int *addr_len)
{
struct inet_sock *inet = inet_sk(sk);
struct sockaddr_in *sin = (struct sockaddr_in *)msg->msg_name;
struct sk_buff *skb;
unsigned int ulen, copied;
int peeked, off = 0;
int err;
int is_udplite = IS_UDPLITE(sk);
bool slow;

	/*
	 *	Check any passed addresses
	 *
	if (addr_len)
		*addr_len = sizeof(*sin);
if (flags & MSG_ERRQUEUE)
return ip_recv_error(sk, msg, len);

try_again:
skb = __skb_recv_datagram(sk, flags | (noblock ? MSG_DONTWAIT : 0),
&peeked, &off, &err);
if (!skb)
goto out;

ulen = skb->len - sizeof(struct udphdr);
copied = len;
if (copied > ulen)
copied = ulen;
else if (copied < ulen)
msg->msg_flags |= MSG_TRUNC;

/*
* If checksum is needed at all, try to do it while copying the
* data.  If the data is truncated, or if we only want a partial
* coverage checksum (UDP-Lite), do it before the copy.
*/

if (copied < ulen || UDP_SKB_CB(skb)->partial_cov) {
if (udp_lib_checksum_complete(skb))
goto csum_copy_err;
}

if (skb_csum_unnecessary(skb))
err = skb_copy_datagram_iovec(skb, sizeof(struct udphdr),
msg->msg_iov, copied);
else {
err = skb_copy_and_csum_datagram_iovec(skb,
sizeof(struct udphdr),
msg->msg_iov);

if (err == -EINVAL)
goto csum_copy_err;
}

if (unlikely(err)) {
trace_kfree_skb(skb, udp_recvmsg);
if (!peeked) {
atomic_inc(&sk->sk_drops);
UDP_INC_STATS_USER(sock_net(sk),
UDP_MIB_INERRORS, is_udplite);
}
goto out_free;
}

if (!peeked)
UDP_INC_STATS_USER(sock_net(sk),
UDP_MIB_INDATAGRAMS, is_udplite);

sock_recv_ts_and_drops(msg, sk, skb);

/* Copy the address. */
if (sin) {
sin->sin_family = AF_INET;
sin->sin_port = udp_hdr(skb)->source;
sin->sin_addr.s_addr = ip_hdr(skb)->saddr;
memset(sin->sin_zero, 0, sizeof(sin->sin_zero));
}
if (inet->cmsg_flags)
ip_cmsg_recv(msg, skb);

err = copied;
if (flags & MSG_TRUNC)
err = ulen;

out_free:
skb_free_datagram_locked(sk, skb);
out:
return err;

csum_copy_err:
slow = lock_sock_fast(sk);
if (!skb_kill_datagram(sk, skb, flags)) {
UDP_INC_STATS_USER(sock_net(sk), UDP_MIB_CSUMERRORS, is_udplite);
UDP_INC_STATS_USER(sock_net(sk), UDP_MIB_INERRORS, is_udplite);
}
unlock_sock_fast(sk, slow);

if (noblock)
return -EAGAIN;

/* starting over for a new packet */
msg->msg_flags &= ~MSG_TRUNC;
goto try_again;
}
