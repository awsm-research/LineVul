static int snd_timer_user_open(struct inode *inode, struct file *file)
{
struct snd_timer_user *tu;
int err;

err = nonseekable_open(inode, file);
if (err < 0)
return err;

tu = kzalloc(sizeof(*tu), GFP_KERNEL);
if (tu == NULL)
return -ENOMEM;
spin_lock_init(&tu->qlock);
init_waitqueue_head(&tu->qchange_sleep);
	mutex_init(&tu->tread_sem);
tu->ticks = 1;
tu->queue_size = 128;
tu->queue = kmalloc(tu->queue_size * sizeof(struct snd_timer_read),
GFP_KERNEL);
if (tu->queue == NULL) {
kfree(tu);
return -ENOMEM;
}
file->private_data = tu;
return 0;
}
