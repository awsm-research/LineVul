long arch_ptrace(struct task_struct *child, long request,
unsigned long addr, unsigned long data)
{
int ret;
unsigned long __user *datap = (unsigned long __user *) data;

switch (request) {
case PTRACE_PEEKUSR:
ret = ptrace_read_user(child, addr, datap);
break;

case PTRACE_POKEUSR:
ret = ptrace_write_user(child, addr, data);
break;

case PTRACE_GETREGS:
ret = copy_regset_to_user(child,
&user_arm_view, REGSET_GPR,
0, sizeof(struct pt_regs),
datap);
break;

case PTRACE_SETREGS:
ret = copy_regset_from_user(child,
&user_arm_view, REGSET_GPR,
0, sizeof(struct pt_regs),
datap);
break;

case PTRACE_GETFPREGS:
ret = copy_regset_to_user(child,
&user_arm_view, REGSET_FPR,
0, sizeof(union fp_state),
datap);
break;

case PTRACE_SETFPREGS:
ret = copy_regset_from_user(child,
&user_arm_view, REGSET_FPR,
0, sizeof(union fp_state),
datap);
break;

#ifdef CONFIG_IWMMXT
case PTRACE_GETWMMXREGS:
ret = ptrace_getwmmxregs(child, datap);
break;

case PTRACE_SETWMMXREGS:
ret = ptrace_setwmmxregs(child, datap);
break;
#endif

case PTRACE_GET_THREAD_AREA:
			ret = put_user(task_thread_info(child)->tp_value,
datap);
break;

case PTRACE_SET_SYSCALL:
task_thread_info(child)->syscall = data;
ret = 0;
break;

#ifdef CONFIG_CRUNCH
case PTRACE_GETCRUNCHREGS:
ret = ptrace_getcrunchregs(child, datap);
break;

case PTRACE_SETCRUNCHREGS:
ret = ptrace_setcrunchregs(child, datap);
break;
#endif

#ifdef CONFIG_VFP
case PTRACE_GETVFPREGS:
ret = copy_regset_to_user(child,
&user_arm_view, REGSET_VFP,
0, ARM_VFPREGS_SIZE,
datap);
break;

case PTRACE_SETVFPREGS:
ret = copy_regset_from_user(child,
&user_arm_view, REGSET_VFP,
0, ARM_VFPREGS_SIZE,
datap);
break;
#endif

#ifdef CONFIG_HAVE_HW_BREAKPOINT
case PTRACE_GETHBPREGS:
if (ptrace_get_breakpoints(child) < 0)
return -ESRCH;

ret = ptrace_gethbpregs(child, addr,
(unsigned long __user *)data);
ptrace_put_breakpoints(child);
break;
case PTRACE_SETHBPREGS:
if (ptrace_get_breakpoints(child) < 0)
return -ESRCH;

ret = ptrace_sethbpregs(child, addr,
(unsigned long __user *)data);
ptrace_put_breakpoints(child);
break;
#endif

default:
ret = ptrace_request(child, request, addr, data);
break;
}

return ret;
}
