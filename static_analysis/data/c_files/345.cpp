static void __net_exit sctp_net_exit(struct net *net)
{
/* Free the local address list */
sctp_free_addr_wq(net);
sctp_free_local_addr_list(net);

	/* Free the control endpoint.  *
	inet_ctl_sock_destroy(net->sctp.ctl_sock);
sctp_dbg_objcnt_exit(net);

sctp_proc_exit(net);
cleanup_sctp_mibs(net);
sctp_sysctl_net_unregister(net);
}
