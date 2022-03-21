static long ion_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
struct ion_client *client = filp->private_data;
struct ion_device *dev = client->dev;
struct ion_handle *cleanup_handle = NULL;
int ret = 0;
unsigned int dir;

union {
struct ion_fd_data fd;
struct ion_allocation_data allocation;
struct ion_handle_data handle;
struct ion_custom_data custom;
} data;

dir = ion_ioctl_dir(cmd);

if (_IOC_SIZE(cmd) > sizeof(data))
return -EINVAL;

if (dir & _IOC_WRITE)
if (copy_from_user(&data, (void __user *)arg, _IOC_SIZE(cmd)))
return -EFAULT;

switch (cmd) {
case ION_IOC_ALLOC:
{
struct ion_handle *handle;

handle = ion_alloc(client, data.allocation.len,
data.allocation.align,
data.allocation.heap_id_mask,
data.allocation.flags);
if (IS_ERR(handle))
return PTR_ERR(handle);

data.allocation.handle = handle->id;

cleanup_handle = handle;
break;
}
case ION_IOC_FREE:
{
struct ion_handle *handle;

		handle = ion_handle_get_by_id(client, data.handle.handle);
		if (IS_ERR(handle))
return PTR_ERR(handle);
		ion_free(client, handle);
		ion_handle_put(handle);
break;
}
case ION_IOC_SHARE:
case ION_IOC_MAP:
{
struct ion_handle *handle;

handle = ion_handle_get_by_id(client, data.handle.handle);
if (IS_ERR(handle))
return PTR_ERR(handle);
data.fd.fd = ion_share_dma_buf_fd(client, handle);
ion_handle_put(handle);
if (data.fd.fd < 0)
ret = data.fd.fd;
break;
}
case ION_IOC_IMPORT:
{
struct ion_handle *handle;

handle = ion_import_dma_buf_fd(client, data.fd.fd);
if (IS_ERR(handle))
ret = PTR_ERR(handle);
else
data.handle.handle = handle->id;
break;
}
case ION_IOC_SYNC:
{
ret = ion_sync_for_device(client, data.fd.fd);
break;
}
case ION_IOC_CUSTOM:
{
if (!dev->custom_ioctl)
return -ENOTTY;
ret = dev->custom_ioctl(client, data.custom.cmd,
data.custom.arg);
break;
}
default:
return -ENOTTY;
}

if (dir & _IOC_READ) {
if (copy_to_user((void __user *)arg, &data, _IOC_SIZE(cmd))) {
if (cleanup_handle)
ion_free(client, cleanup_handle);
return -EFAULT;
}
}
return ret;
}
