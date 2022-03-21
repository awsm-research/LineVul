static struct block_device *ext3_blkdev_get(dev_t dev, struct super_block *sb)
{
struct block_device *bdev;
char b[BDEVNAME_SIZE];

bdev = blkdev_get_by_dev(dev, FMODE_READ|FMODE_WRITE|FMODE_EXCL, sb);
if (IS_ERR(bdev))
goto fail;
return bdev;

fail:
	ext3_msg(sb, "error: failed to open journal device %s: %ld",
__bdevname(dev, b), PTR_ERR(bdev));

return NULL;
}
