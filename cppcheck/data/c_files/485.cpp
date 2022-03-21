static void bt_for_each(struct blk_mq_hw_ctx *hctx,
struct blk_mq_bitmap_tags *bt, unsigned int off,
busy_iter_fn *fn, void *data, bool reserved)
{
struct request *rq;
int bit, i;

for (i = 0; i < bt->map_nr; i++) {
struct blk_align_bitmap *bm = &bt->map[i];

for (bit = find_first_bit(&bm->word, bm->depth);
bit < bm->depth;
bit = find_next_bit(&bm->word, bm->depth, bit + 1)) {
		     	rq = blk_mq_tag_to_rq(hctx->tags, off + bit);
if (rq->q == hctx->queue)
fn(hctx, rq, data, reserved);
}

off += (1 << bt->bits_per_word);
}
}
