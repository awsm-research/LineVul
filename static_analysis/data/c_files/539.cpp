static int attach_child_main(void* data)
{
struct attach_clone_payload* payload = (struct attach_clone_payload*)data;
int ipc_socket = payload->ipc_socket;
	int procfd = payload->procfd;
lxc_attach_options_t* options = payload->options;
struct lxc_proc_context_info* init_ctx = payload->init_ctx;
#if HAVE_SYS_PERSONALITY_H
long new_personality;
#endif
int ret;
int status;
int expected;
long flags;
int fd;
uid_t new_uid;
gid_t new_gid;

/* wait for the initial thread to signal us that it's ready
* for us to start initializing
*/
expected = 0;
status = -1;
ret = lxc_read_nointr_expect(ipc_socket, &status, sizeof(status), &expected);
if (ret <= 0) {
		ERROR("error using IPC to receive notification from initial process (0)");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}

/* A description of the purpose of this functionality is
* provided in the lxc-attach(1) manual page. We have to
* remount here and not in the parent process, otherwise
* /proc may not properly reflect the new pid namespace.
*/
if (!(options->namespaces & CLONE_NEWNS) && (options->attach_flags & LXC_ATTACH_REMOUNT_PROC_SYS)) {
ret = lxc_attach_remount_sys_proc();
if (ret < 0) {
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}
}

/* now perform additional attachments*/
#if HAVE_SYS_PERSONALITY_H
if (options->personality < 0)
new_personality = init_ctx->personality;
else
new_personality = options->personality;

if (options->attach_flags & LXC_ATTACH_SET_PERSONALITY) {
ret = personality(new_personality);
if (ret < 0) {
SYSERROR("could not ensure correct architecture");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}
}
#endif

if (options->attach_flags & LXC_ATTACH_DROP_CAPABILITIES) {
ret = lxc_attach_drop_privs(init_ctx);
if (ret < 0) {
ERROR("could not drop privileges");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}
}

/* always set the environment (specify (LXC_ATTACH_KEEP_ENV, NULL, NULL) if you want this to be a no-op) */
ret = lxc_attach_set_environment(options->env_policy, options->extra_env_vars, options->extra_keep_env);
if (ret < 0) {
ERROR("could not set initial environment for attached process");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}

/* set user / group id */
new_uid = 0;
new_gid = 0;
/* ignore errors, we will fall back to root in that case
* (/proc was not mounted etc.)
*/
if (options->namespaces & CLONE_NEWUSER)
lxc_attach_get_init_uidgid(&new_uid, &new_gid);

if (options->uid != (uid_t)-1)
new_uid = options->uid;
if (options->gid != (gid_t)-1)
new_gid = options->gid;

/* setup the control tty */
if (options->stdin_fd && isatty(options->stdin_fd)) {
if (setsid() < 0) {
SYSERROR("unable to setsid");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}

if (ioctl(options->stdin_fd, TIOCSCTTY, (char *)NULL) < 0) {
SYSERROR("unable to TIOCSTTY");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}
}

/* try to set the uid/gid combination */
if ((new_gid != 0 || options->namespaces & CLONE_NEWUSER)) {
if (setgid(new_gid) || setgroups(0, NULL)) {
SYSERROR("switching to container gid");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}
}
if ((new_uid != 0 || options->namespaces & CLONE_NEWUSER) && setuid(new_uid)) {
SYSERROR("switching to container uid");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}

/* tell initial process it may now put us into the cgroups */
status = 1;
ret = lxc_write_nointr(ipc_socket, &status, sizeof(status));
if (ret != sizeof(status)) {
		ERROR("error using IPC to notify initial process for initialization (1)");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}

/* wait for the initial thread to signal us that it has done
* everything for us when it comes to cgroups etc.
*/
expected = 2;
status = -1;
ret = lxc_read_nointr_expect(ipc_socket, &status, sizeof(status), &expected);
if (ret <= 0) {
		ERROR("error using IPC to receive final notification from initial process (2)");
shutdown(ipc_socket, SHUT_RDWR);
rexit(-1);
}

	shutdown(ipc_socket, SHUT_RDWR);
	close(ipc_socket);
if ((init_ctx->container && init_ctx->container->lxc_conf &&
init_ctx->container->lxc_conf->no_new_privs) ||
(options->attach_flags & LXC_ATTACH_NO_NEW_PRIVS)) {
if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
SYSERROR("PR_SET_NO_NEW_PRIVS could not be set. "
"Process can use execve() gainable "
"privileges.");
rexit(-1);
}
INFO("PR_SET_NO_NEW_PRIVS is set. Process cannot use execve() "
"gainable privileges.");
}

	/* set new apparmor profile/selinux context *
if ((options->namespaces & CLONE_NEWNS) && (options->attach_flags & LXC_ATTACH_LSM) && init_ctx->lsm_label) {
int on_exec;

on_exec = options->attach_flags & LXC_ATTACH_LSM_EXEC ? 1 : 0;
		if (lsm_set_label_at(procfd, on_exec, init_ctx->lsm_label) < 0) {
rexit(-1);
}
}
if (init_ctx->container && init_ctx->container->lxc_conf &&
init_ctx->container->lxc_conf->seccomp &&
(lxc_seccomp_load(init_ctx->container->lxc_conf) != 0)) {
ERROR("Loading seccomp policy");
rexit(-1);
}
lxc_proc_put_context_info(init_ctx);

/* The following is done after the communication socket is
* shut down. That way, all errors that might (though
* unlikely) occur up until this point will have their messages
* printed to the original stderr (if logging is so configured)
* and not the fd the user supplied, if any.
*/

/* fd handling for stdin, stdout and stderr;
* ignore errors here, user may want to make sure
* the fds are closed, for example */
if (options->stdin_fd >= 0 && options->stdin_fd != 0)
dup2(options->stdin_fd, 0);
if (options->stdout_fd >= 0 && options->stdout_fd != 1)
dup2(options->stdout_fd, 1);
if (options->stderr_fd >= 0 && options->stderr_fd != 2)
dup2(options->stderr_fd, 2);

/* close the old fds */
if (options->stdin_fd > 2)
close(options->stdin_fd);
if (options->stdout_fd > 2)
close(options->stdout_fd);
if (options->stderr_fd > 2)
close(options->stderr_fd);

/* try to remove CLOEXEC flag from stdin/stdout/stderr,
* but also here, ignore errors */
for (fd = 0; fd <= 2; fd++) {
flags = fcntl(fd, F_GETFL);
if (flags < 0)
continue;
if (flags & FD_CLOEXEC)
if (fcntl(fd, F_SETFL, flags & ~FD_CLOEXEC) < 0)
SYSERROR("Unable to clear CLOEXEC from fd");
}

	/* we don't need proc anymore *
	close(procfd);
/* we're done, so we can now do whatever the user intended us to do */
rexit(payload->exec_function(payload->exec_payload));
}
