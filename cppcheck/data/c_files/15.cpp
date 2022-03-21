static ssize_t snd_timer_user_read(struct file *file, char __user *buffer,
size_t count, loff_t *offset)
{
struct snd_timer_user *tu;
long result = 0, unit;
int qhead;
int err = 0;

tu = file->private_data;
unit = tu->tread ? sizeof(struct snd_timer_tread) : sizeof(struct snd_timer_read);
spin_lock_irq(&tu->qlock);
while ((long)count - result >= unit) {
while (!tu->qused) {
wait_queue_t wait;

if ((file->f_flags & O_NONBLOCK) != 0 || result > 0) {
err = -EAGAIN;
goto _error;
}

set_current_state(TASK_INTERRUPTIBLE);
init_waitqueue_entry(&wait, current);
add_wait_queue(&tu->qchange_sleep, &wait);

spin_unlock_irq(&tu->qlock);
schedule();
spin_lock_irq(&tu->qlock);

remove_wait_queue(&tu->qchange_sleep, &wait);

if (tu->disconnected) {
err = -ENODEV;
goto _error;
}
if (signal_pending(current)) {
err = -ERESTARTSYS;
goto _error;
}
}

qhead = tu->qhead++;
tu->qhead %= tu->queue_size;
tu->qused--;
spin_unlock_irq(&tu->qlock);

		mutex_lock(&tu->ioctl_lock);
if (tu->tread) {
if (copy_to_user(buffer, &tu->tqueue[qhead],
sizeof(struct snd_timer_tread)))
err = -EFAULT;
} else {
if (copy_to_user(buffer, &tu->queue[qhead],
sizeof(struct snd_timer_read)))
err = -EFAULT;
}
		mutex_unlock(&tu->ioctl_lock);

spin_lock_irq(&tu->qlock);
if (err < 0)
goto _error;
result += unit;
buffer += unit;
}
_error:
spin_unlock_irq(&tu->qlock);
return result > 0 ? result : err;
}
