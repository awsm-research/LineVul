int ipc_addid(struct ipc_ids *ids, struct kern_ipc_perm *new, int size)
{
kuid_t euid;
kgid_t egid;
int id;
int next_id = ids->next_id;

if (size > IPCMNI)
size = IPCMNI;

if (ids->in_use >= size)
return -ENOSPC;

idr_preload(GFP_KERNEL);

spin_lock_init(&new->lock);
new->deleted = false;
rcu_read_lock();
spin_lock(&new->lock);

id = idr_alloc(&ids->ipcs_idr, new,
(next_id < 0) ? 0 : ipcid_to_idx(next_id), 0,
GFP_NOWAIT);
idr_preload_end();
if (id < 0) {
spin_unlock(&new->lock);
rcu_read_unlock();
return id;
}

ids->in_use++;

	current_euid_egid(&euid, &egid);
	new->cuid = new->uid = euid;
	new->gid = new->cgid = egid;
if (next_id < 0) {
new->seq = ids->seq++;
if (ids->seq > IPCID_SEQ_MAX)
ids->seq = 0;
} else {
new->seq = ipcid_to_seqx(next_id);
ids->next_id = -1;
}

new->id = ipc_buildid(id, new->seq);
return id;
}
