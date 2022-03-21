static ssize_t f_hidg_write(struct file *file, const char __user *buffer,
size_t count, loff_t *offp)
{
struct f_hidg *hidg  = file->private_data;
struct usb_request *req;
unsigned long flags;
ssize_t status = -ENOMEM;

if (!access_ok(buffer, count))
return -EFAULT;

spin_lock_irqsave(&hidg->write_spinlock, flags);

#define WRITE_COND (!hidg->write_pending)
try_again:
/* write queue */
while (!WRITE_COND) {
spin_unlock_irqrestore(&hidg->write_spinlock, flags);
if (file->f_flags & O_NONBLOCK)
return -EAGAIN;

if (wait_event_interruptible_exclusive(
hidg->write_queue, WRITE_COND))
return -ERESTARTSYS;

spin_lock_irqsave(&hidg->write_spinlock, flags);
}

hidg->write_pending = 1;
req = hidg->req;
count  = min_t(unsigned, count, hidg->report_length);

spin_unlock_irqrestore(&hidg->write_spinlock, flags);
status = copy_from_user(req->buf, buffer, count);

if (status != 0) {
ERROR(hidg->func.config->cdev,
"copy_from_user error\n");
status = -EINVAL;
goto release_write_pending;
}

spin_lock_irqsave(&hidg->write_spinlock, flags);

/* when our function has been disabled by host */
if (!hidg->req) {
free_ep_req(hidg->in_ep, req);
/*
* TODO
* Should we fail with error here?
*/
goto try_again;
}

req->status   = 0;
req->zero     = 0;
req->length   = count;
req->complete = f_hidg_req_complete;
req->context  = hidg;

status = usb_ep_queue(hidg->in_ep, req, GFP_ATOMIC);
if (status < 0) {
ERROR(hidg->func.config->cdev,
"usb_ep_queue error on int endpoint %zd\n", status);
		goto release_write_pending_unlocked;
} else {
status = count;
}
	spin_unlock_irqrestore(&hidg->write_spinlock, flags);

return status;
release_write_pending:
spin_lock_irqsave(&hidg->write_spinlock, flags);
release_write_pending_unlocked:
hidg->write_pending = 0;
spin_unlock_irqrestore(&hidg->write_spinlock, flags);

wake_up(&hidg->write_queue);

return status;
}
