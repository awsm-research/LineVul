void ide_dma_cb(void *opaque, int ret)
{
IDEState *s = opaque;
int n;
int64_t sector_num;
bool stay_active = false;

if (ret == -ECANCELED) {
return;
}
if (ret < 0) {
int op = IDE_RETRY_DMA;

if (s->dma_cmd == IDE_DMA_READ)
op |= IDE_RETRY_READ;
else if (s->dma_cmd == IDE_DMA_TRIM)
op |= IDE_RETRY_TRIM;

if (ide_handle_rw_error(s, -ret, op)) {
return;
}
}

n = s->io_buffer_size >> 9;
if (n > s->nsector) {
/* The PRDs were longer than needed for this request. Shorten them so
* we don't get a negative remainder. The Active bit must remain set
* after the request completes. */
n = s->nsector;
stay_active = true;
}

sector_num = ide_get_sector(s);
if (n > 0) {
assert(s->io_buffer_size == s->sg.size);
dma_buf_commit(s, s->io_buffer_size);
sector_num += n;
ide_set_sector(s, sector_num);
s->nsector -= n;
}

/* end of transfer ? */
if (s->nsector == 0) {
s->status = READY_STAT | SEEK_STAT;
ide_set_irq(s->bus);
goto eot;
}

/* launch next transfer */
n = s->nsector;
s->io_buffer_index = 0;
s->io_buffer_size = n * 512;
    if (s->bus->dma->ops->prepare_buf(s->bus->dma, ide_cmd_is_read(s)) == 0) {
/* The PRDs were too short. Reset the Active bit, but don't raise an
* interrupt. */
s->status = READY_STAT | SEEK_STAT;
goto eot;
}

printf("ide_dma_cb: sector_num=%" PRId64 " n=%d, cmd_cmd=%d\n",
sector_num, n, s->dma_cmd);
#endif

if ((s->dma_cmd == IDE_DMA_READ || s->dma_cmd == IDE_DMA_WRITE) &&
!ide_sect_range_ok(s, sector_num, n)) {
ide_dma_error(s);
return;
}

switch (s->dma_cmd) {
case IDE_DMA_READ:
s->bus->dma->aiocb = dma_blk_read(s->blk, &s->sg, sector_num,
ide_dma_cb, s);
break;
case IDE_DMA_WRITE:
s->bus->dma->aiocb = dma_blk_write(s->blk, &s->sg, sector_num,
ide_dma_cb, s);
break;
case IDE_DMA_TRIM:
s->bus->dma->aiocb = dma_blk_io(s->blk, &s->sg, sector_num,
ide_issue_trim, ide_dma_cb, s,
DMA_DIRECTION_TO_DEVICE);
break;
}
return;

eot:
if (s->dma_cmd == IDE_DMA_READ || s->dma_cmd == IDE_DMA_WRITE) {
block_acct_done(blk_get_stats(s->blk), &s->acct);
}
ide_set_inactive(s, stay_active);
}
