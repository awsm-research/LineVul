static void put_crypt_info(struct fscrypt_info *ci)
{
if (!ci)
return;

	key_put(ci->ci_keyring_key);
crypto_free_skcipher(ci->ci_ctfm);
kmem_cache_free(fscrypt_info_cachep, ci);
}
