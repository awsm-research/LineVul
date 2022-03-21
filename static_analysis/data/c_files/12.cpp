key_ref_t key_create_or_update(key_ref_t keyring_ref,
const char *type,
const char *description,
const void *payload,
size_t plen,
key_perm_t perm,
unsigned long flags)
{
struct keyring_index_key index_key = {
.description	= description,
};
struct key_preparsed_payload prep;
struct assoc_array_edit *edit;
const struct cred *cred = current_cred();
struct key *keyring, *key = NULL;
key_ref_t key_ref;
int ret;

/* look up the key type to see if it's one of the registered kernel
* types */
index_key.type = key_type_lookup(type);
if (IS_ERR(index_key.type)) {
key_ref = ERR_PTR(-ENODEV);
goto error;
}

key_ref = ERR_PTR(-EINVAL);
	if (!index_key.type->match || !index_key.type->instantiate ||
(!index_key.description && !index_key.type->preparse))
goto error_put_type;

keyring = key_ref_to_ptr(keyring_ref);

key_check(keyring);

key_ref = ERR_PTR(-ENOTDIR);
if (keyring->type != &key_type_keyring)
goto error_put_type;

memset(&prep, 0, sizeof(prep));
prep.data = payload;
prep.datalen = plen;
prep.quotalen = index_key.type->def_datalen;
prep.trusted = flags & KEY_ALLOC_TRUSTED;
prep.expiry = TIME_T_MAX;
if (index_key.type->preparse) {
ret = index_key.type->preparse(&prep);
if (ret < 0) {
key_ref = ERR_PTR(ret);
goto error_free_prep;
}
if (!index_key.description)
index_key.description = prep.description;
key_ref = ERR_PTR(-EINVAL);
if (!index_key.description)
goto error_free_prep;
}
index_key.desc_len = strlen(index_key.description);

key_ref = ERR_PTR(-EPERM);
if (!prep.trusted && test_bit(KEY_FLAG_TRUSTED_ONLY, &keyring->flags))
goto error_free_prep;
flags |= prep.trusted ? KEY_ALLOC_TRUSTED : 0;

ret = __key_link_begin(keyring, &index_key, &edit);
if (ret < 0) {
key_ref = ERR_PTR(ret);
goto error_free_prep;
}

/* if we're going to allocate a new key, we're going to have
* to modify the keyring */
ret = key_permission(keyring_ref, KEY_NEED_WRITE);
if (ret < 0) {
key_ref = ERR_PTR(ret);
goto error_link_end;
}

/* if it's possible to update this type of key, search for an existing
* key of the same type and description in the destination keyring and
* update that instead if possible
*/
if (index_key.type->update) {
key_ref = find_key_to_update(keyring_ref, &index_key);
if (key_ref)
goto found_matching_key;
}

/* if the client doesn't provide, decide on the permissions we want */
if (perm == KEY_PERM_UNDEF) {
perm = KEY_POS_VIEW | KEY_POS_SEARCH | KEY_POS_LINK | KEY_POS_SETATTR;
perm |= KEY_USR_VIEW;

if (index_key.type->read)
perm |= KEY_POS_READ;

if (index_key.type == &key_type_keyring ||
index_key.type->update)
perm |= KEY_POS_WRITE;
}

/* allocate a new key */
key = key_alloc(index_key.type, index_key.description,
cred->fsuid, cred->fsgid, cred, perm, flags);
if (IS_ERR(key)) {
key_ref = ERR_CAST(key);
goto error_link_end;
}

/* instantiate it and link it into the target keyring */
ret = __key_instantiate_and_link(key, &prep, keyring, NULL, &edit);
if (ret < 0) {
key_put(key);
key_ref = ERR_PTR(ret);
goto error_link_end;
}

key_ref = make_key_ref(key, is_key_possessed(keyring_ref));

error_link_end:
__key_link_end(keyring, &index_key, edit);
error_free_prep:
if (index_key.type->preparse)
index_key.type->free_preparse(&prep);
error_put_type:
key_type_put(index_key.type);
error:
return key_ref;

found_matching_key:
/* we found a matching key, so we're going to try to update it
* - we can drop the locks first as we have the key pinned
*/
__key_link_end(keyring, &index_key, edit);

key_ref = __key_update(key_ref, &prep);
goto error_free_prep;
}
