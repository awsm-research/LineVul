int snd_timer_close(struct snd_timer_instance *timeri)
{
struct snd_timer *timer = NULL;
struct snd_timer_instance *slave, *tmp;

if (snd_BUG_ON(!timeri))
return -ENXIO;

/* force to stop the timer */
snd_timer_stop(timeri);

if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE) {
/* wait, until the active callback is finished */
spin_lock_irq(&slave_active_lock);
while (timeri->flags & SNDRV_TIMER_IFLG_CALLBACK) {
spin_unlock_irq(&slave_active_lock);
udelay(10);
spin_lock_irq(&slave_active_lock);
}
spin_unlock_irq(&slave_active_lock);
mutex_lock(&register_mutex);
list_del(&timeri->open_list);
mutex_unlock(&register_mutex);
} else {
timer = timeri->timer;
if (snd_BUG_ON(!timer))
goto out;
/* wait, until the active callback is finished */
spin_lock_irq(&timer->lock);
while (timeri->flags & SNDRV_TIMER_IFLG_CALLBACK) {
spin_unlock_irq(&timer->lock);
udelay(10);
spin_lock_irq(&timer->lock);
}
spin_unlock_irq(&timer->lock);
mutex_lock(&register_mutex);
list_del(&timeri->open_list);
if (timer && list_empty(&timer->open_list_head) &&
timer->hw.close)
timer->hw.close(timer);
/* remove slave links */
list_for_each_entry_safe(slave, tmp, &timeri->slave_list_head,
open_list) {
			spin_lock_irq(&slave_active_lock);
			_snd_timer_stop(slave, 1, SNDRV_TIMER_EVENT_RESOLUTION);
list_move_tail(&slave->open_list, &snd_timer_slave_list);
slave->master = NULL;
slave->timer = NULL;
			spin_unlock_irq(&slave_active_lock);
}
mutex_unlock(&register_mutex);
}
out:
if (timeri->private_free)
timeri->private_free(timeri);
kfree(timeri->owner);
kfree(timeri);
if (timer)
module_put(timer->module);
return 0;
}
