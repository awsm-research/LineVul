static int msg_cache_check(const char *id, struct BodyCache *bcache, void *data)
{
struct Context *ctx = (struct Context *) data;
if (!ctx)
return -1;
struct PopData *pop_data = (struct PopData *) ctx->data;
if (!pop_data)
return -1;

#ifdef USE_HCACHE
/* keep hcache file if hcache == bcache */
if (strcmp(HC_FNAME "." HC_FEXT, id) == 0)
return 0;
#endif

for (int i = 0; i < ctx->msgcount; i++)
{
/* if the id we get is known for a header: done (i.e. keep in cache) */
if (ctx->hdrs[i]->data && (mutt_str_strcmp(ctx->hdrs[i]->data, id) == 0))
return 0;
}

/* message not found in context -> remove it from cache
* return the result of bcache, so we stop upon its first error
*/
  return mutt_bcache_del(bcache, id);
}
