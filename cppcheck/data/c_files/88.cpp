int ext4_orphan_del(handle_t *handle, struct inode *inode)
{
struct list_head *prev;
struct ext4_inode_info *ei = EXT4_I(inode);
struct ext4_sb_info *sbi;
__u32 ino_next;
struct ext4_iloc iloc;
int err = 0;

	if (!EXT4_SB(inode->i_sb)->s_journal)
return 0;

mutex_lock(&EXT4_SB(inode->i_sb)->s_orphan_lock);
if (list_empty(&ei->i_orphan))
goto out;

ino_next = NEXT_ORPHAN(inode);
prev = ei->i_orphan.prev;
sbi = EXT4_SB(inode->i_sb);

jbd_debug(4, "remove inode %lu from orphan list\n", inode->i_ino);

list_del_init(&ei->i_orphan);

/* If we're on an error path, we may not have a valid
* transaction handle with which to update the orphan list on
* disk, but we still need to remove the inode from the linked
* list in memory. */
if (!handle)
goto out;

err = ext4_reserve_inode_write(handle, inode, &iloc);
if (err)
goto out_err;

if (prev == &sbi->s_orphan) {
jbd_debug(4, "superblock will point to %u\n", ino_next);
BUFFER_TRACE(sbi->s_sbh, "get_write_access");
err = ext4_journal_get_write_access(handle, sbi->s_sbh);
if (err)
goto out_brelse;
sbi->s_es->s_last_orphan = cpu_to_le32(ino_next);
err = ext4_handle_dirty_super(handle, inode->i_sb);
} else {
struct ext4_iloc iloc2;
struct inode *i_prev =
&list_entry(prev, struct ext4_inode_info, i_orphan)->vfs_inode;

jbd_debug(4, "orphan inode %lu will point to %u\n",
i_prev->i_ino, ino_next);
err = ext4_reserve_inode_write(handle, i_prev, &iloc2);
if (err)
goto out_brelse;
NEXT_ORPHAN(i_prev) = ino_next;
err = ext4_mark_iloc_dirty(handle, i_prev, &iloc2);
}
if (err)
goto out_brelse;
NEXT_ORPHAN(inode) = 0;
err = ext4_mark_iloc_dirty(handle, inode, &iloc);

out_err:
ext4_std_error(inode->i_sb, err);
out:
mutex_unlock(&EXT4_SB(inode->i_sb)->s_orphan_lock);
return err;

out_brelse:
brelse(iloc.bh);
goto out_err;
}
