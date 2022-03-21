asmlinkage long compat_sys_recvmmsg(int fd, struct compat_mmsghdr __user *mmsg,
unsigned int vlen, unsigned int flags,
struct compat_timespec __user *timeout)
{
int datagrams;
struct timespec ktspec;

if (flags & MSG_CMSG_COMPAT)
return -EINVAL;

	if (COMPAT_USE_64BIT_TIME)
		return __sys_recvmmsg(fd, (struct mmsghdr __user *)mmsg, vlen,
				      flags | MSG_CMSG_COMPAT,
				      (struct timespec *) timeout);
if (timeout == NULL)
return __sys_recvmmsg(fd, (struct mmsghdr __user *)mmsg, vlen,
flags | MSG_CMSG_COMPAT, NULL);

	if (get_compat_timespec(&ktspec, timeout))
return -EFAULT;

datagrams = __sys_recvmmsg(fd, (struct mmsghdr __user *)mmsg, vlen,
flags | MSG_CMSG_COMPAT, &ktspec);
	if (datagrams > 0 && put_compat_timespec(&ktspec, timeout))
datagrams = -EFAULT;

return datagrams;
}
