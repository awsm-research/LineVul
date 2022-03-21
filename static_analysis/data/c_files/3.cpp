SYSCALL_DEFINE5(waitid, int, which, pid_t, upid, struct siginfo __user *,
infop, int, options, struct rusage __user *, ru)
{
struct rusage r;
struct waitid_info info = {.status = 0};
long err = kernel_waitid(which, upid, &info, options, ru ? &r : NULL);
int signo = 0;
if (err > 0) {
signo = SIGCHLD;
err = 0;
	}
	if (!err) {
if (ru && copy_to_user(ru, &r, sizeof(struct rusage)))
return -EFAULT;
}
if (!infop)
return err;

user_access_begin();
unsafe_put_user(signo, &infop->si_signo, Efault);
unsafe_put_user(0, &infop->si_errno, Efault);
unsafe_put_user(info.cause, &infop->si_code, Efault);
unsafe_put_user(info.pid, &infop->si_pid, Efault);
unsafe_put_user(info.uid, &infop->si_uid, Efault);
unsafe_put_user(info.status, &infop->si_status, Efault);
user_access_end();
return err;
Efault:
user_access_end();
return -EFAULT;
}
