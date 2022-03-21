SYSCALL_DEFINE6(move_pages, pid_t, pid, unsigned long, nr_pages,
const void __user * __user *, pages,
const int __user *, nodes,
int __user *, status, int, flags)
{
	const struct cred *cred = current_cred(), *tcred;
struct task_struct *task;
struct mm_struct *mm;
int err;
nodemask_t task_nodes;

/* Check flags */
if (flags & ~(MPOL_MF_MOVE|MPOL_MF_MOVE_ALL))
return -EINVAL;

if ((flags & MPOL_MF_MOVE_ALL) && !capable(CAP_SYS_NICE))
return -EPERM;

/* Find the mm_struct */
rcu_read_lock();
task = pid ? find_task_by_vpid(pid) : current;
if (!task) {
rcu_read_unlock();
return -ESRCH;
}
get_task_struct(task);

/*
* Check if this process has the right to modify the specified
	 * process. The right exists if the process has administrative
	 * capabilities, superuser privileges or the same
	 * userid as the target process.
*/
	tcred = __task_cred(task);
	if (!uid_eq(cred->euid, tcred->suid) && !uid_eq(cred->euid, tcred->uid) &&
	    !uid_eq(cred->uid,  tcred->suid) && !uid_eq(cred->uid,  tcred->uid) &&
	    !capable(CAP_SYS_NICE)) {
rcu_read_unlock();
err = -EPERM;
goto out;
}
rcu_read_unlock();

err = security_task_movememory(task);
if (err)
goto out;

task_nodes = cpuset_mems_allowed(task);
mm = get_task_mm(task);
put_task_struct(task);

if (!mm)
return -EINVAL;

if (nodes)
err = do_pages_move(mm, task_nodes, nr_pages, pages,
nodes, status, flags);
else
err = do_pages_stat(mm, nr_pages, pages, status);

mmput(mm);
return err;

out:
put_task_struct(task);
return err;
}
