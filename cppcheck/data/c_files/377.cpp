static int sctp_wait_for_sndbuf(struct sctp_association *asoc, long *timeo_p,
size_t msg_len)
{
struct sock *sk = asoc->base.sk;
int err = 0;
long current_timeo = *timeo_p;
DEFINE_WAIT(wait);

pr_debug("%s: asoc:%p, timeo:%ld, msg_len:%zu\n", __func__, asoc,
*timeo_p, msg_len);

/* Increment the association's refcnt.  */
sctp_association_hold(asoc);

/* Wait on the association specific sndbuf space. */
for (;;) {
prepare_to_wait_exclusive(&asoc->wait, &wait,
TASK_INTERRUPTIBLE);
if (!*timeo_p)
goto do_nonblock;
if (sk->sk_err || asoc->state >= SCTP_STATE_SHUTDOWN_PENDING ||
asoc->base.dead)
goto do_error;
if (signal_pending(current))
goto do_interrupted;
if (msg_len <= sctp_wspace(asoc))
break;

/* Let another process have a go.  Since we are going
* to sleep anyway.
*/
release_sock(sk);
current_timeo = schedule_timeout(current_timeo);
		BUG_ON(sk != asoc->base.sk);
lock_sock(sk);

*timeo_p = current_timeo;
}

out:
finish_wait(&asoc->wait, &wait);

/* Release the association's refcnt.  */
sctp_association_put(asoc);

return err;

do_error:
err = -EPIPE;
goto out;

do_interrupted:
err = sock_intr_errno(*timeo_p);
goto out;

do_nonblock:
err = -EAGAIN;
goto out;
}
