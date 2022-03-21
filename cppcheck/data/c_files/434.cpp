static int netlink_recvmsg(struct kiocb *kiocb, struct socket *sock,
struct msghdr *msg, size_t len,
int flags)
{
struct sock_iocb *siocb = kiocb_to_siocb(kiocb);
struct scm_cookie scm;
struct sock *sk = sock->sk;
struct netlink_sock *nlk = nlk_sk(sk);
int noblock = flags&MSG_DONTWAIT;
size_t copied;
struct sk_buff *skb, *data_skb;
int err, ret;

if (flags&MSG_OOB)
return -EOPNOTSUPP;

copied = 0;

skb = skb_recv_datagram(sk, flags, noblock, &err);
if (skb == NULL)
goto out;

data_skb = skb;

#ifdef CONFIG_COMPAT_NETLINK_MESSAGES
if (unlikely(skb_shinfo(skb)->frag_list)) {
/*
* If this skb has a frag_list, then here that means that we
* will have to use the frag_list skb's data for compat tasks
* and the regular skb's data for normal (non-compat) tasks.
*
* If we need to send the compat skb, assign it to the
* 'data_skb' variable so that it will be used below for data
* copying. We keep 'skb' for everything else, including
* freeing both later.
*/
if (flags & MSG_CMSG_COMPAT)
data_skb = skb_shinfo(skb)->frag_list;
}
#endif

	msg->msg_namelen = 0;
copied = data_skb->len;
if (len < copied) {
msg->msg_flags |= MSG_TRUNC;
copied = len;
}

skb_reset_transport_header(data_skb);
err = skb_copy_datagram_iovec(data_skb, 0, msg->msg_iov, copied);

if (msg->msg_name) {
struct sockaddr_nl *addr = (struct sockaddr_nl *)msg->msg_name;
addr->nl_family = AF_NETLINK;
addr->nl_pad    = 0;
addr->nl_pid	= NETLINK_CB(skb).portid;
addr->nl_groups	= netlink_group_mask(NETLINK_CB(skb).dst_group);
msg->msg_namelen = sizeof(*addr);
}

if (nlk->flags & NETLINK_RECV_PKTINFO)
netlink_cmsg_recv_pktinfo(msg, skb);

if (NULL == siocb->scm) {
memset(&scm, 0, sizeof(scm));
siocb->scm = &scm;
}
siocb->scm->creds = *NETLINK_CREDS(skb);
if (flags & MSG_TRUNC)
copied = data_skb->len;

skb_free_datagram(sk, skb);

if (nlk->cb_running &&
atomic_read(&sk->sk_rmem_alloc) <= sk->sk_rcvbuf / 2) {
ret = netlink_dump(sk);
if (ret) {
sk->sk_err = ret;
sk->sk_error_report(sk);
}
}

scm_recv(sock, msg, siocb->scm, flags);
out:
netlink_rcv_wake(sk);
return err ? : copied;
}
