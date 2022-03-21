ext4_xattr_block_get(struct inode *inode, int name_index, const char *name,
void *buffer, size_t buffer_size)
{
struct buffer_head *bh = NULL;
struct ext4_xattr_entry *entry;
size_t size;
int error;
	struct mb_cache *ext4_mb_cache = EXT4_GET_MB_CACHE(inode);

ea_idebug(inode, "name=%d.%s, buffer=%p, buffer_size=%ld",
name_index, name, buffer, (long)buffer_size);

error = -ENODATA;
if (!EXT4_I(inode)->i_file_acl)
goto cleanup;
ea_idebug(inode, "reading block %llu",
(unsigned long long)EXT4_I(inode)->i_file_acl);
bh = sb_bread(inode->i_sb, EXT4_I(inode)->i_file_acl);
if (!bh)
goto cleanup;
ea_bdebug(bh, "b_count=%d, refcount=%d",
atomic_read(&(bh->b_count)), le32_to_cpu(BHDR(bh)->h_refcount));
if (ext4_xattr_check_block(inode, bh)) {
bad_block:
EXT4_ERROR_INODE(inode, "bad block %llu",
EXT4_I(inode)->i_file_acl);
error = -EFSCORRUPTED;
goto cleanup;
}
ext4_xattr_cache_insert(ext4_mb_cache, bh);
entry = BFIRST(bh);
error = ext4_xattr_find_entry(&entry, name_index, name, bh->b_size, 1);
if (error == -EFSCORRUPTED)
goto bad_block;
if (error)
goto cleanup;
size = le32_to_cpu(entry->e_value_size);
if (buffer) {
error = -ERANGE;
if (size > buffer_size)
goto cleanup;
memcpy(buffer, bh->b_data + le16_to_cpu(entry->e_value_offs),
size);
}
error = size;

cleanup:
brelse(bh);
return error;
}
