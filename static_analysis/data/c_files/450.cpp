static struct mount *clone_mnt(struct mount *old, struct dentry *root,
int flag)
{
struct super_block *sb = old->mnt.mnt_sb;
struct mount *mnt;
int err;

mnt = alloc_vfsmnt(old->mnt_devname);
if (!mnt)
return ERR_PTR(-ENOMEM);

if (flag & (CL_SLAVE | CL_PRIVATE | CL_SHARED_TO_SLAVE))
mnt->mnt_group_id = 0; /* not a peer of original */
else
mnt->mnt_group_id = old->mnt_group_id;

if ((flag & CL_MAKE_SHARED) && !mnt->mnt_group_id) {
err = mnt_alloc_group_id(mnt);
if (err)
goto out_free;
}

mnt->mnt.mnt_flags = old->mnt.mnt_flags & ~(MNT_WRITE_HOLD|MNT_MARKED);
/* Don't allow unprivileged users to change mount flags */
	if ((flag & CL_UNPRIVILEGED) && (mnt->mnt.mnt_flags & MNT_READONLY))
		mnt->mnt.mnt_flags |= MNT_LOCK_READONLY;

/* Don't allow unprivileged users to reveal what is under a mount */
if ((flag & CL_UNPRIVILEGED) && list_empty(&old->mnt_expire))
mnt->mnt.mnt_flags |= MNT_LOCKED;

atomic_inc(&sb->s_active);
mnt->mnt.mnt_sb = sb;
mnt->mnt.mnt_root = dget(root);
mnt->mnt_mountpoint = mnt->mnt.mnt_root;
mnt->mnt_parent = mnt;
lock_mount_hash();
list_add_tail(&mnt->mnt_instance, &sb->s_mounts);
unlock_mount_hash();

if ((flag & CL_SLAVE) ||
((flag & CL_SHARED_TO_SLAVE) && IS_MNT_SHARED(old))) {
list_add(&mnt->mnt_slave, &old->mnt_slave_list);
mnt->mnt_master = old;
CLEAR_MNT_SHARED(mnt);
} else if (!(flag & CL_PRIVATE)) {
if ((flag & CL_MAKE_SHARED) || IS_MNT_SHARED(old))
list_add(&mnt->mnt_share, &old->mnt_share);
if (IS_MNT_SLAVE(old))
list_add(&mnt->mnt_slave, &old->mnt_slave);
mnt->mnt_master = old->mnt_master;
}
if (flag & CL_MAKE_SHARED)
set_mnt_shared(mnt);

/* stick the duplicate mount on the same expiry list
* as the original if that was on one */
if (flag & CL_EXPIRE) {
if (!list_empty(&old->mnt_expire))
list_add(&mnt->mnt_expire, &old->mnt_expire);
}

return mnt;

out_free:
mnt_free_id(mnt);
free_vfsmnt(mnt);
return ERR_PTR(err);
}
