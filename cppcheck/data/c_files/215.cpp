static void scsi_read_data(SCSIRequest *req)
{
SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);
SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);
uint32_t n;

if (r->sector_count == (uint32_t)-1) {
DPRINTF("Read buf_len=%zd\n", r->iov.iov_len);
r->sector_count = 0;
scsi_req_data(&r->req, r->iov.iov_len);
return;
}
DPRINTF("Read sector_count=%d\n", r->sector_count);
if (r->sector_count == 0) {
/* This also clears the sense buffer for REQUEST SENSE.  */
scsi_req_complete(&r->req, GOOD);
return;
}

/* No data transfer may already be in progress */
assert(r->req.aiocb == NULL);

if (r->req.cmd.mode == SCSI_XFER_TO_DEV) {
DPRINTF("Data transfer direction invalid\n");
scsi_read_complete(r, -EINVAL);
return;
}

    n = r->sector_count;
    if (n > SCSI_DMA_BUF_SIZE / 512)
        n = SCSI_DMA_BUF_SIZE / 512;
if (s->tray_open) {
scsi_read_complete(r, -ENOMEDIUM);
}
    r->iov.iov_len = n * 512;
    qemu_iovec_init_external(&r->qiov, &r->iov, 1);
bdrv_acct_start(s->bs, &r->acct, n * BDRV_SECTOR_SIZE, BDRV_ACCT_READ);
r->req.aiocb = bdrv_aio_readv(s->bs, r->sector, &r->qiov, n,
scsi_read_complete, r);
if (r->req.aiocb == NULL) {
scsi_read_complete(r, -EIO);
}
}
