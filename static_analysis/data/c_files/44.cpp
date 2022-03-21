queue_unlock(struct futex_q *q, struct futex_hash_bucket *hb)
{
spin_unlock(&hb->lock);
	drop_futex_key_refs(&q->key);
}
