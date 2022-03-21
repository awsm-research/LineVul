void __detach_mounts(struct dentry *dentry)
{
struct mountpoint *mp;
struct mount *mnt;

namespace_lock();
mp = lookup_mountpoint(dentry);
if (IS_ERR_OR_NULL(mp))
goto out_unlock;

lock_mount_hash();
while (!hlist_empty(&mp->m_list)) {
mnt = hlist_entry(mp->m_list.first, struct mount, mnt_mp_list);
if (mnt->mnt.mnt_flags & MNT_UMOUNT) {
struct mount *p, *tmp;
list_for_each_entry_safe(p, tmp, &mnt->mnt_mounts,  mnt_child) {
hlist_add_head(&p->mnt_umount.s_list, &unmounted);
umount_mnt(p);
}
}
		else umount_tree(mnt, 0);
}
unlock_mount_hash();
put_mountpoint(mp);
out_unlock:
namespace_unlock();
}
