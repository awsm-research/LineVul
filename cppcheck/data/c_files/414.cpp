static int futex_wait_requeue_pi(u32 __user *uaddr, int fshared,
u32 val, ktime_t *abs_time, u32 bitset,
int clockrt, u32 __user *uaddr2)
{
struct hrtimer_sleeper timeout, *to = NULL;
struct rt_mutex_waiter rt_waiter;
struct rt_mutex *pi_mutex = NULL;
struct futex_hash_bucket *hb;
union futex_key key2;
struct futex_q q;
int res, ret;

if (!bitset)
return -EINVAL;

if (abs_time) {
to = &timeout;
hrtimer_init_on_stack(&to->timer, clockrt ? CLOCK_REALTIME :
CLOCK_MONOTONIC, HRTIMER_MODE_ABS);
hrtimer_init_sleeper(to, current);
hrtimer_set_expires_range_ns(&to->timer, *abs_time,
current->timer_slack_ns);
}

/*
* The waiter is allocated on our stack, manipulated by the requeue
* code while we sleep on uaddr.
*/
debug_rt_mutex_init_waiter(&rt_waiter);
rt_waiter.task = NULL;

key2 = FUTEX_KEY_INIT;
ret = get_futex_key(uaddr2, fshared, &key2);
if (unlikely(ret != 0))
goto out;

q.pi_state = NULL;
q.bitset = bitset;
q.rt_waiter = &rt_waiter;
q.requeue_pi_key = &key2;

	/* Prepare to wait on uaddr. *
ret = futex_wait_setup(uaddr, val, fshared, &q, &hb);
if (ret)
goto out_key2;

/* Queue the futex_q, drop the hb lock, wait for wakeup. */
futex_wait_queue_me(hb, &q, to);

spin_lock(&hb->lock);
ret = handle_early_requeue_pi_wakeup(hb, &q, &key2, to);
spin_unlock(&hb->lock);
if (ret)
goto out_put_keys;

/*
* In order for us to be here, we know our q.key == key2, and since
* we took the hb->lock above, we also know that futex_requeue() has
* completed and we no longer have to concern ourselves with a wakeup
	 * race with the atomic proxy lock acquition by the requeue code.
*/

/* Check if the requeue code acquired the second futex for us. */
if (!q.rt_waiter) {
/*
* Got the lock. We might not be the anticipated owner if we
* did a lock-steal - fix up the PI-state in that case.
*/
if (q.pi_state && (q.pi_state->owner != current)) {
spin_lock(q.lock_ptr);
ret = fixup_pi_state_owner(uaddr2, &q, current,
fshared);
spin_unlock(q.lock_ptr);
}
} else {
/*
* We have been woken up by futex_unlock_pi(), a timeout, or a
* signal.  futex_unlock_pi() will not destroy the lock_ptr nor
* the pi_state.
*/
WARN_ON(!&q.pi_state);
pi_mutex = &q.pi_state->pi_mutex;
ret = rt_mutex_finish_proxy_lock(pi_mutex, to, &rt_waiter, 1);
debug_rt_mutex_free_waiter(&rt_waiter);

spin_lock(q.lock_ptr);
/*
* Fixup the pi_state owner and possibly acquire the lock if we
* haven't already.
*/
res = fixup_owner(uaddr2, fshared, &q, !ret);
/*
* If fixup_owner() returned an error, proprogate that.  If it
* acquired the lock, clear -ETIMEDOUT or -EINTR.
*/
if (res)
ret = (res < 0) ? res : 0;

/* Unqueue and drop the lock. */
unqueue_me_pi(&q);
}

/*
* If fixup_pi_state_owner() faulted and was unable to handle the
* fault, unlock the rt_mutex and return the fault to userspace.
*/
if (ret == -EFAULT) {
if (rt_mutex_owner(pi_mutex) == current)
rt_mutex_unlock(pi_mutex);
} else if (ret == -EINTR) {
/*
* We've already been requeued, but cannot restart by calling
* futex_lock_pi() directly. We could restart this syscall, but
* it would detect that the user space "val" changed and return
* -EWOULDBLOCK.  Save the overhead of the restart and return
* -EWOULDBLOCK directly.
*/
ret = -EWOULDBLOCK;
}

out_put_keys:
put_futex_key(fshared, &q.key);
out_key2:
put_futex_key(fshared, &key2);

out:
if (to) {
hrtimer_cancel(&to->timer);
destroy_hrtimer_on_stack(&to->timer);
}
return ret;
}
