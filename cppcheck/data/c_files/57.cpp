static int __ptrace_may_access(struct task_struct *task, unsigned int mode)
{
const struct cred *cred = current_cred(), *tcred;

/* May we inspect the given task?
* This check is used both for attaching with ptrace
* and for allowing access to sensitive information in /proc.
*
* ptrace_attach denies several cases that /proc allows
* because setting up the necessary parent/child relationship
* or halting the specified task is impossible.
*/
int dumpable = 0;
/* Don't let security modules deny introspection */
if (same_thread_group(task, current))
return 0;
rcu_read_lock();
tcred = __task_cred(task);
if (uid_eq(cred->uid, tcred->euid) &&
uid_eq(cred->uid, tcred->suid) &&
uid_eq(cred->uid, tcred->uid)  &&
gid_eq(cred->gid, tcred->egid) &&
gid_eq(cred->gid, tcred->sgid) &&
gid_eq(cred->gid, tcred->gid))
goto ok;
if (ptrace_has_cap(tcred->user_ns, mode))
goto ok;
rcu_read_unlock();
return -EPERM;
ok:
rcu_read_unlock();
smp_rmb();
if (task->mm)
dumpable = get_dumpable(task->mm);
rcu_read_lock();
	if (!dumpable && !ptrace_has_cap(__task_cred(task)->user_ns, mode)) {
rcu_read_unlock();
return -EPERM;
}
rcu_read_unlock();

return security_ptrace_access_check(task, mode);
}
