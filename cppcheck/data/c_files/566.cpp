static int ocfs2_dio_get_block(struct inode *inode, sector_t iblock,
struct buffer_head *bh_result, int create)
{
struct ocfs2_super *osb = OCFS2_SB(inode->i_sb);
struct ocfs2_inode_info *oi = OCFS2_I(inode);
struct ocfs2_write_ctxt *wc;
struct ocfs2_write_cluster_desc *desc = NULL;
struct ocfs2_dio_write_ctxt *dwc = NULL;
struct buffer_head *di_bh = NULL;
u64 p_blkno;
loff_t pos = iblock << inode->i_sb->s_blocksize_bits;
unsigned len, total_len = bh_result->b_size;
int ret = 0, first_get_block = 0;

len = osb->s_clustersize - (pos & (osb->s_clustersize - 1));
len = min(total_len, len);

mlog(0, "get block of %lu at %llu:%u req %u\n",
inode->i_ino, pos, len, total_len);

/*
* Because we need to change file size in ocfs2_dio_end_io_write(), or
* we may need to add it to orphan dir. So can not fall to fast path
* while file size will be changed.
*/
if (pos + total_len <= i_size_read(inode)) {
		down_read(&oi->ip_alloc_sem);
		/* This is the fast path for re-write. *
		ret = ocfs2_get_block(inode, iblock, bh_result, create);
		up_read(&oi->ip_alloc_sem);

if (buffer_mapped(bh_result) &&
!buffer_new(bh_result) &&
ret == 0)
goto out;

/* Clear state set by ocfs2_get_block. */
bh_result->b_state = 0;
}

dwc = ocfs2_dio_alloc_write_ctx(bh_result, &first_get_block);
if (unlikely(dwc == NULL)) {
ret = -ENOMEM;
mlog_errno(ret);
goto out;
}

if (ocfs2_clusters_for_bytes(inode->i_sb, pos + total_len) >
ocfs2_clusters_for_bytes(inode->i_sb, i_size_read(inode)) &&
!dwc->dw_orphaned) {
/*
* when we are going to alloc extents beyond file size, add the
* inode to orphan dir, so we can recall those spaces when
* system crashed during write.
*/
ret = ocfs2_add_inode_to_orphan(osb, inode);
if (ret < 0) {
mlog_errno(ret);
goto out;
}
dwc->dw_orphaned = 1;
}

ret = ocfs2_inode_lock(inode, &di_bh, 1);
if (ret) {
mlog_errno(ret);
goto out;
}

down_write(&oi->ip_alloc_sem);

if (first_get_block) {
if (ocfs2_sparse_alloc(OCFS2_SB(inode->i_sb)))
ret = ocfs2_zero_tail(inode, di_bh, pos);
else
ret = ocfs2_expand_nonsparse_inode(inode, di_bh, pos,
total_len, NULL);
if (ret < 0) {
mlog_errno(ret);
goto unlock;
}
}

ret = ocfs2_write_begin_nolock(inode->i_mapping, pos, len,
OCFS2_WRITE_DIRECT, NULL,
(void **)&wc, di_bh, NULL);
if (ret) {
mlog_errno(ret);
goto unlock;
}

desc = &wc->w_desc[0];

p_blkno = ocfs2_clusters_to_blocks(inode->i_sb, desc->c_phys);
BUG_ON(p_blkno == 0);
p_blkno += iblock & (u64)(ocfs2_clusters_to_blocks(inode->i_sb, 1) - 1);

map_bh(bh_result, inode->i_sb, p_blkno);
bh_result->b_size = len;
if (desc->c_needs_zero)
set_buffer_new(bh_result);

/* May sleep in end_io. It should not happen in a irq context. So defer
* it to dio work queue. */
set_buffer_defer_completion(bh_result);

if (!list_empty(&wc->w_unwritten_list)) {
struct ocfs2_unwritten_extent *ue = NULL;

ue = list_first_entry(&wc->w_unwritten_list,
struct ocfs2_unwritten_extent,
ue_node);
BUG_ON(ue->ue_cpos != desc->c_cpos);
/* The physical address may be 0, fill it. */
ue->ue_phys = desc->c_phys;

list_splice_tail_init(&wc->w_unwritten_list, &dwc->dw_zero_list);
dwc->dw_zero_count++;
}

ret = ocfs2_write_end_nolock(inode->i_mapping, pos, len, len, wc);
BUG_ON(ret != len);
ret = 0;
unlock:
up_write(&oi->ip_alloc_sem);
ocfs2_inode_unlock(inode, 1);
brelse(di_bh);
out:
if (ret < 0)
ret = -EIO;
return ret;
}
