static void shm_destroy(struct ipc_namespace *ns, struct shmid_kernel *shp)
{
ns->shm_tot -= (shp->shm_segsz + PAGE_SIZE - 1) >> PAGE_SHIFT;
shm_rmid(ns, shp);
shm_unlock(shp);
	if (!is_file_hugepages(shp->shm_file))
		shmem_lock(shp->shm_file, 0, shp->mlock_user);
else if (shp->mlock_user)
		user_shm_unlock(file_inode(shp->shm_file)->i_size,
						shp->mlock_user);
	fput (shp->shm_file);
ipc_rcu_putref(shp, shm_rcu_free);
}
