static int __rds_rdma_map(struct rds_sock *rs, struct rds_get_mr_args *args,
u64 *cookie_ret, struct rds_mr **mr_ret)
{
struct rds_mr *mr = NULL, *found;
unsigned int nr_pages;
struct page **pages = NULL;
struct scatterlist *sg;
void *trans_private;
unsigned long flags;
rds_rdma_cookie_t cookie;
unsigned int nents;
long i;
int ret;

	if (rs->rs_bound_addr == 0) {
ret = -ENOTCONN; /* XXX not a great errno */
goto out;
}

if (!rs->rs_transport->get_mr) {
ret = -EOPNOTSUPP;
goto out;
}

nr_pages = rds_pages_in_vec(&args->vec);
if (nr_pages == 0) {
ret = -EINVAL;
goto out;
}

/* Restrict the size of mr irrespective of underlying transport
* To account for unaligned mr regions, subtract one from nr_pages
*/
if ((nr_pages - 1) > (RDS_MAX_MSG_SIZE >> PAGE_SHIFT)) {
ret = -EMSGSIZE;
goto out;
}

rdsdebug("RDS: get_mr addr %llx len %llu nr_pages %u\n",
args->vec.addr, args->vec.bytes, nr_pages);

/* XXX clamp nr_pages to limit the size of this alloc? */
pages = kcalloc(nr_pages, sizeof(struct page *), GFP_KERNEL);
if (!pages) {
ret = -ENOMEM;
goto out;
}

mr = kzalloc(sizeof(struct rds_mr), GFP_KERNEL);
if (!mr) {
ret = -ENOMEM;
goto out;
}

refcount_set(&mr->r_refcount, 1);
RB_CLEAR_NODE(&mr->r_rb_node);
mr->r_trans = rs->rs_transport;
mr->r_sock = rs;

if (args->flags & RDS_RDMA_USE_ONCE)
mr->r_use_once = 1;
if (args->flags & RDS_RDMA_INVALIDATE)
mr->r_invalidate = 1;
if (args->flags & RDS_RDMA_READWRITE)
mr->r_write = 1;

/*
* Pin the pages that make up the user buffer and transfer the page
* pointers to the mr's sg array.  We check to see if we've mapped
* the whole region after transferring the partial page references
* to the sg array so that we can have one page ref cleanup path.
*
* For now we have no flag that tells us whether the mapping is
* r/o or r/w. We need to assume r/w, or we'll do a lot of RDMA to
* the zero page.
*/
ret = rds_pin_pages(args->vec.addr, nr_pages, pages, 1);
if (ret < 0)
goto out;

nents = ret;
sg = kcalloc(nents, sizeof(*sg), GFP_KERNEL);
if (!sg) {
ret = -ENOMEM;
goto out;
}
WARN_ON(!nents);
sg_init_table(sg, nents);

/* Stick all pages into the scatterlist */
for (i = 0 ; i < nents; i++)
sg_set_page(&sg[i], pages[i], PAGE_SIZE, 0);

rdsdebug("RDS: trans_private nents is %u\n", nents);

/* Obtain a transport specific MR. If this succeeds, the
* s/g list is now owned by the MR.
* Note that dma_map() implies that pending writes are
* flushed to RAM, so no dma_sync is needed here. */
trans_private = rs->rs_transport->get_mr(sg, nents, rs,
&mr->r_key);

if (IS_ERR(trans_private)) {
for (i = 0 ; i < nents; i++)
put_page(sg_page(&sg[i]));
kfree(sg);
ret = PTR_ERR(trans_private);
goto out;
}

mr->r_trans_private = trans_private;

rdsdebug("RDS: get_mr put_user key is %x cookie_addr %p\n",
mr->r_key, (void *)(unsigned long) args->cookie_addr);

/* The user may pass us an unaligned address, but we can only
* map page aligned regions. So we keep the offset, and build
* a 64bit cookie containing <R_Key, offset> and pass that
* around. */
cookie = rds_rdma_make_cookie(mr->r_key, args->vec.addr & ~PAGE_MASK);
if (cookie_ret)
*cookie_ret = cookie;

if (args->cookie_addr && put_user(cookie, (u64 __user *)(unsigned long) args->cookie_addr)) {
ret = -EFAULT;
goto out;
}

/* Inserting the new MR into the rbtree bumps its
* reference count. */
spin_lock_irqsave(&rs->rs_rdma_lock, flags);
found = rds_mr_tree_walk(&rs->rs_rdma_keys, mr->r_key, mr);
spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);

BUG_ON(found && found != mr);

rdsdebug("RDS: get_mr key is %x\n", mr->r_key);
if (mr_ret) {
refcount_inc(&mr->r_refcount);
*mr_ret = mr;
}

ret = 0;
out:
kfree(pages);
if (mr)
rds_mr_put(mr);
return ret;
}
