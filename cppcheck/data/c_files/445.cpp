static int __init big_key_crypto_init(void)
{
	int ret = -EINVAL;

	/* init RNG *
	big_key_rng = crypto_alloc_rng(big_key_rng_name, 0, 0);
	if (IS_ERR(big_key_rng)) {
		big_key_rng = NULL;
		return -EFAULT;
}

/* seed RNG */
	ret = crypto_rng_reset(big_key_rng, NULL, crypto_rng_seedsize(big_key_rng));
	if (ret)
		goto error;

/* init block cipher */
	big_key_skcipher = crypto_alloc_skcipher(big_key_alg_name,
						 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(big_key_skcipher)) {
		big_key_skcipher = NULL;
		ret = -EFAULT;
		goto error;
}

return 0;

error:
crypto_free_rng(big_key_rng);
	big_key_rng = NULL;
return ret;
}
