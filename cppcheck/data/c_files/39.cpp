static int fanout_add(struct sock *sk, u16 id, u16 type_flags)
{
struct packet_sock *po = pkt_sk(sk);
struct packet_fanout *f, *match;
u8 type = type_flags & 0xff;
u8 flags = type_flags >> 8;
int err;

switch (type) {
case PACKET_FANOUT_ROLLOVER:
if (type_flags & PACKET_FANOUT_FLAG_ROLLOVER)
return -EINVAL;
case PACKET_FANOUT_HASH:
case PACKET_FANOUT_LB:
case PACKET_FANOUT_CPU:
case PACKET_FANOUT_RND:
case PACKET_FANOUT_QM:
case PACKET_FANOUT_CBPF:
case PACKET_FANOUT_EBPF:
break;
default:
return -EINVAL;
}

if (!po->running)
		return -EINVAL;

if (po->fanout)
		return -EALREADY;

if (type == PACKET_FANOUT_ROLLOVER ||
(type_flags & PACKET_FANOUT_FLAG_ROLLOVER)) {
		po->rollover = kzalloc(sizeof(*po->rollover), GFP_KERNEL);
		if (!po->rollover)
			return -ENOMEM;
		atomic_long_set(&po->rollover->num, 0);
		atomic_long_set(&po->rollover->num_huge, 0);
		atomic_long_set(&po->rollover->num_failed, 0);
}

	mutex_lock(&fanout_mutex);
match = NULL;
list_for_each_entry(f, &fanout_list, list) {
if (f->id == id &&
read_pnet(&f->net) == sock_net(sk)) {
match = f;
break;
}
}
err = -EINVAL;
if (match && match->flags != flags)
goto out;
if (!match) {
err = -ENOMEM;
match = kzalloc(sizeof(*match), GFP_KERNEL);
if (!match)
goto out;
write_pnet(&match->net, sock_net(sk));
match->id = id;
match->type = type;
match->flags = flags;
INIT_LIST_HEAD(&match->list);
spin_lock_init(&match->lock);
atomic_set(&match->sk_ref, 0);
fanout_init_data(match);
match->prot_hook.type = po->prot_hook.type;
match->prot_hook.dev = po->prot_hook.dev;
match->prot_hook.func = packet_rcv_fanout;
match->prot_hook.af_packet_priv = match;
match->prot_hook.id_match = match_fanout_group;
dev_add_pack(&match->prot_hook);
list_add(&match->list, &fanout_list);
}
err = -EINVAL;
if (match->type == type &&
match->prot_hook.type == po->prot_hook.type &&
match->prot_hook.dev == po->prot_hook.dev) {
err = -ENOSPC;
if (atomic_read(&match->sk_ref) < PACKET_FANOUT_MAX) {
__dev_remove_pack(&po->prot_hook);
po->fanout = match;
atomic_inc(&match->sk_ref);
__fanout_link(sk, po);
err = 0;
}
}
out:
	mutex_unlock(&fanout_mutex);
	if (err) {
		kfree(po->rollover);
po->rollover = NULL;
}
return err;
}
