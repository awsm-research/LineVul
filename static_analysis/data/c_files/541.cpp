static int update_open_stateid(struct nfs4_state *state, nfs4_stateid *open_stateid, nfs4_stateid *delegation, int open_flags)
{
struct nfs_inode *nfsi = NFS_I(state->inode);
struct nfs_delegation *deleg_cur;
int ret = 0;

	open_flags &= (FMODE_READ|FMODE_WRITE);

rcu_read_lock();
deleg_cur = rcu_dereference(nfsi->delegation);
if (deleg_cur == NULL)
goto no_delegation;

spin_lock(&deleg_cur->lock);
if (nfsi->delegation != deleg_cur ||
	    (deleg_cur->type & open_flags) != open_flags)
goto no_delegation_unlock;

if (delegation == NULL)
delegation = &deleg_cur->stateid;
else if (memcmp(deleg_cur->stateid.data, delegation->data, NFS4_STATEID_SIZE) != 0)
goto no_delegation_unlock;

nfs_mark_delegation_referenced(deleg_cur);
	__update_open_stateid(state, open_stateid, &deleg_cur->stateid, open_flags);
ret = 1;
no_delegation_unlock:
spin_unlock(&deleg_cur->lock);
no_delegation:
rcu_read_unlock();

if (!ret && open_stateid != NULL) {
		__update_open_stateid(state, open_stateid, NULL, open_flags);
ret = 1;
}

return ret;
}
