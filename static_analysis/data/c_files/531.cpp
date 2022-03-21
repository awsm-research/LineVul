static inline void sem_getref_and_unlock(struct sem_array *sma)
{
	ipc_rcu_getref(sma);
	ipc_unlock(&(sma)->sem_perm);
}
