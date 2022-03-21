static ps_sd *ps_sd_new(ps_mm *data, const char *key)
{
php_uint32 hv, slot;
ps_sd *sd;
int keylen;

keylen = strlen(key);

sd = mm_malloc(data->mm, sizeof(ps_sd) + keylen);
if (!sd) {
TSRMLS_FETCH();

               php_error_docref(NULL TSRMLS_CC, E_WARNING, "mm_malloc failed, avail %d, err %s", mm_available(data->mm), mm_error());
return NULL;
}

hv = ps_sd_hash(key, keylen);
slot = hv & data->hash_max;

sd->ctime = 0;
sd->hv = hv;
sd->data = NULL;
sd->alloclen = sd->datalen = 0;

memcpy(sd->key, key, keylen + 1);

sd->next = data->hash[slot];
data->hash[slot] = sd;

data->hash_cnt++;

if (!sd->next) {
if (data->hash_cnt >= data->hash_max) {
hash_split(data);
}
}

ps_mm_debug(("inserting %s(%p) into slot %d\n", key, sd, slot));

return sd;
}
