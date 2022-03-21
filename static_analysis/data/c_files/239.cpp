static int ext4_write_end(struct file *file,
struct address_space *mapping,
loff_t pos, unsigned len, unsigned copied,
struct page *page, void *fsdata)
{
handle_t *handle = ext4_journal_current_handle();
struct inode *inode = mapping->host;
loff_t old_size = inode->i_size;
int ret = 0, ret2;
int i_size_changed = 0;

trace_ext4_write_end(inode, pos, len, copied);
	if (ext4_test_inode_state(inode, EXT4_STATE_ORDERED_MODE)) {
		ret = ext4_jbd2_file_inode(handle, inode);
		if (ret) {
			unlock_page(page);
			put_page(page);
			goto errout;
		}
	}
if (ext4_has_inline_data(inode)) {
ret = ext4_write_inline_data_end(inode, pos, len,
copied, page);
if (ret < 0)
goto errout;
copied = ret;
} else
copied = block_write_end(file, mapping, pos,
len, copied, page, fsdata);
/*
* it's important to update i_size while still holding page lock:
* page writeout could otherwise come in and zero beyond i_size.
*/
i_size_changed = ext4_update_inode_size(inode, pos + copied);
unlock_page(page);
put_page(page);

if (old_size < pos)
pagecache_isize_extended(inode, old_size, pos);
/*
* Don't mark the inode dirty under page lock. First, it unnecessarily
* makes the holding time of page lock longer. Second, it forces lock
* ordering of page lock and transaction start for journaling
* filesystems.
*/
if (i_size_changed)
ext4_mark_inode_dirty(handle, inode);

if (pos + len > inode->i_size && ext4_can_truncate(inode))
/* if we have allocated more blocks and copied
* less. We will have blocks allocated outside
* inode->i_size. So truncate them
*/
ext4_orphan_add(handle, inode);
errout:
ret2 = ext4_journal_stop(handle);
if (!ret)
ret = ret2;

if (pos + len > inode->i_size) {
ext4_truncate_failed_write(inode);
/*
* If truncate failed early the inode might still be
* on the orphan list; we need to make sure the inode
* is removed from the orphan list in that case.
*/
if (inode->i_nlink)
ext4_orphan_del(NULL, inode);
}

return ret ? ret : copied;
}
