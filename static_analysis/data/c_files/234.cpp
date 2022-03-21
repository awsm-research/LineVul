static inline void process_get_command(conn *c, token_t *tokens, size_t ntokens, bool return_cas) {
char *key;
size_t nkey;
int i = 0;
item *it;
token_t *key_token = &tokens[KEY_TOKEN];
char *suffix;
assert(c != NULL);

do {
while(key_token->length != 0) {

key = key_token->value;
nkey = key_token->length;

if(nkey > KEY_MAX_LENGTH) {
out_string(c, "CLIENT_ERROR bad command line format");
while (i-- > 0) {
item_remove(*(c->ilist + i));
}
return;
}

            it = item_get(key, nkey, c, DO_UPDATE);
if (settings.detail_enabled) {
stats_prefix_record_get(key, nkey, NULL != it);
}
if (it) {
if (i >= c->isize) {
item **new_list = realloc(c->ilist, sizeof(item *) * c->isize * 2);
if (new_list) {
c->isize *= 2;
c->ilist = new_list;
} else {
STATS_LOCK();
stats.malloc_fails++;
STATS_UNLOCK();
item_remove(it);
break;
}
}

/*
* Construct the response. Each hit adds three elements to the
* outgoing data list:
*   "VALUE "
*   key
*   " " + flags + " " + data length + "\r\n" + data (with \r\n)
*/

if (return_cas || !settings.inline_ascii_response)
{
MEMCACHED_COMMAND_GET(c->sfd, ITEM_key(it), it->nkey,
it->nbytes, ITEM_get_cas(it));
/* Goofy mid-flight realloc. */
if (i >= c->suffixsize) {
char **new_suffix_list = realloc(c->suffixlist,
sizeof(char *) * c->suffixsize * 2);
if (new_suffix_list) {
c->suffixsize *= 2;
c->suffixlist  = new_suffix_list;
} else {
STATS_LOCK();
stats.malloc_fails++;
STATS_UNLOCK();
item_remove(it);
break;
}
}

suffix = do_cache_alloc(c->thread->suffix_cache);
if (suffix == NULL) {
STATS_LOCK();
stats.malloc_fails++;
STATS_UNLOCK();
out_of_memory(c, "SERVER_ERROR out of memory making CAS suffix");
item_remove(it);
while (i-- > 0) {
item_remove(*(c->ilist + i));
}
return;
}
*(c->suffixlist + i) = suffix;
int suffix_len = make_ascii_get_suffix(suffix, it, return_cas);
if (add_iov(c, "VALUE ", 6) != 0 ||
add_iov(c, ITEM_key(it), it->nkey) != 0 ||
(settings.inline_ascii_response && add_iov(c, ITEM_suffix(it), it->nsuffix - 2) != 0) ||
add_iov(c, suffix, suffix_len) != 0)
{
item_remove(it);
break;
}
if ((it->it_flags & ITEM_CHUNKED) == 0) {
add_iov(c, ITEM_data(it), it->nbytes);
} else if (add_chunked_item_iovs(c, it, it->nbytes) != 0) {
item_remove(it);
break;
}
}
else
{
MEMCACHED_COMMAND_GET(c->sfd, ITEM_key(it), it->nkey,
it->nbytes, ITEM_get_cas(it));
if (add_iov(c, "VALUE ", 6) != 0 ||
add_iov(c, ITEM_key(it), it->nkey) != 0)
{
item_remove(it);
break;
}
if ((it->it_flags & ITEM_CHUNKED) == 0)
{
if (add_iov(c, ITEM_suffix(it), it->nsuffix + it->nbytes) != 0)
{
item_remove(it);
break;
}
} else if (add_iov(c, ITEM_suffix(it), it->nsuffix) != 0 ||
add_chunked_item_iovs(c, it, it->nbytes) != 0) {
item_remove(it);
break;
}
}


if (settings.verbose > 1) {
int ii;
fprintf(stderr, ">%d sending key ", c->sfd);
for (ii = 0; ii < it->nkey; ++ii) {
fprintf(stderr, "%c", key[ii]);
}
fprintf(stderr, "\n");
}

/* item_get() has incremented it->refcount for us */
pthread_mutex_lock(&c->thread->stats.mutex);
c->thread->stats.slab_stats[ITEM_clsid(it)].get_hits++;
c->thread->stats.get_cmds++;
pthread_mutex_unlock(&c->thread->stats.mutex);
*(c->ilist + i) = it;
i++;

} else {
pthread_mutex_lock(&c->thread->stats.mutex);
c->thread->stats.get_misses++;
c->thread->stats.get_cmds++;
pthread_mutex_unlock(&c->thread->stats.mutex);
MEMCACHED_COMMAND_GET(c->sfd, key, nkey, -1, 0);
}

key_token++;
}

/*
* If the command string hasn't been fully processed, get the next set
* of tokens.
*/
if(key_token->value != NULL) {
ntokens = tokenize_command(key_token->value, tokens, MAX_TOKENS);
key_token = tokens;
}

} while(key_token->value != NULL);

c->icurr = c->ilist;
c->ileft = i;
if (return_cas || !settings.inline_ascii_response) {
c->suffixcurr = c->suffixlist;
c->suffixleft = i;
}

if (settings.verbose > 1)
fprintf(stderr, ">%d END\n", c->sfd);

/*
If the loop was terminated because of out-of-memory, it is not
reliable to add END\r\n to the buffer, because it might not end
in \r\n. So we send SERVER_ERROR instead.
*/
if (key_token->value != NULL || add_iov(c, "END\r\n", 5) != 0
|| (IS_UDP(c->transport) && build_udp_headers(c) != 0)) {
out_of_memory(c, "SERVER_ERROR out of memory writing get response");
}
else {
conn_set_state(c, conn_mwrite);
c->msgcurr = 0;
}
}
