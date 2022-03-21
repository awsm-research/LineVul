xfs_attr_rmtval_set(
struct xfs_da_args	*args)
{
struct xfs_inode	*dp = args->dp;
struct xfs_mount	*mp = dp->i_mount;
struct xfs_bmbt_irec	map;
xfs_dablk_t		lblkno;
xfs_fileoff_t		lfileoff = 0;
__uint8_t		*src = args->value;
int			blkcnt;
int			valuelen;
int			nmap;
int			error;
int			offset = 0;

trace_xfs_attr_rmtval_set(args);

/*
* Find a "hole" in the attribute address space large enough for
* us to drop the new attribute's value into. Because CRC enable
* attributes have headers, we can't just do a straight byte to FSB
* conversion and have to take the header space into account.
*/
	blkcnt = xfs_attr3_rmt_blocks(mp, args->valuelen);
error = xfs_bmap_first_unused(args->trans, args->dp, blkcnt, &lfileoff,
XFS_ATTR_FORK);
if (error)
return error;

args->rmtblkno = lblkno = (xfs_dablk_t)lfileoff;
args->rmtblkcnt = blkcnt;

/*
* Roll through the "value", allocating blocks on disk as required.
*/
while (blkcnt > 0) {
int	committed;

/*
* Allocate a single extent, up to the size of the value.
*/
xfs_bmap_init(args->flist, args->firstblock);
nmap = 1;
error = xfs_bmapi_write(args->trans, dp, (xfs_fileoff_t)lblkno,
blkcnt,
XFS_BMAPI_ATTRFORK | XFS_BMAPI_METADATA,
args->firstblock, args->total, &map, &nmap,
args->flist);
if (!error) {
error = xfs_bmap_finish(&args->trans, args->flist,
&committed);
}
if (error) {
ASSERT(committed);
args->trans = NULL;
xfs_bmap_cancel(args->flist);
return(error);
}

/*
* bmap_finish() may have committed the last trans and started
* a new one.  We need the inode to be in all transactions.
*/
if (committed)
xfs_trans_ijoin(args->trans, dp, 0);

ASSERT(nmap == 1);
ASSERT((map.br_startblock != DELAYSTARTBLOCK) &&
(map.br_startblock != HOLESTARTBLOCK));
lblkno += map.br_blockcount;
blkcnt -= map.br_blockcount;

/*
* Start the next trans in the chain.
*/
error = xfs_trans_roll(&args->trans, dp);
if (error)
return (error);
}

/*
* Roll through the "value", copying the attribute value to the
* already-allocated blocks.  Blocks are written synchronously
* so that we can know they are all on disk before we turn off
* the INCOMPLETE flag.
*/
lblkno = args->rmtblkno;
blkcnt = args->rmtblkcnt;
	valuelen = args->valuelen;
while (valuelen > 0) {
struct xfs_buf	*bp;
xfs_daddr_t	dblkno;
int		dblkcnt;

ASSERT(blkcnt > 0);

xfs_bmap_init(args->flist, args->firstblock);
nmap = 1;
error = xfs_bmapi_read(dp, (xfs_fileoff_t)lblkno,
blkcnt, &map, &nmap,
XFS_BMAPI_ATTRFORK);
if (error)
return(error);
ASSERT(nmap == 1);
ASSERT((map.br_startblock != DELAYSTARTBLOCK) &&
(map.br_startblock != HOLESTARTBLOCK));

dblkno = XFS_FSB_TO_DADDR(mp, map.br_startblock),
dblkcnt = XFS_FSB_TO_BB(mp, map.br_blockcount);

bp = xfs_buf_get(mp->m_ddev_targp, dblkno, dblkcnt, 0);
if (!bp)
return ENOMEM;
bp->b_ops = &xfs_attr3_rmt_buf_ops;

xfs_attr_rmtval_copyin(mp, bp, args->dp->i_ino, &offset,
&valuelen, &src);

error = xfs_bwrite(bp);	/* GROT: NOTE: synchronous write */
xfs_buf_relse(bp);
if (error)
return error;


/* roll attribute extent map forwards */
lblkno += map.br_blockcount;
blkcnt -= map.br_blockcount;
}
ASSERT(valuelen == 0);
return 0;
}
