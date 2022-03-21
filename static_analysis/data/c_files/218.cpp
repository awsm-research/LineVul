static int crypto_report_cipher(struct sk_buff *skb, struct crypto_alg *alg)
{
struct crypto_report_cipher rcipher;

	snprintf(rcipher.type, CRYPTO_MAX_ALG_NAME, "%s", "cipher");

rcipher.blocksize = alg->cra_blocksize;
rcipher.min_keysize = alg->cra_cipher.cia_min_keysize;
rcipher.max_keysize = alg->cra_cipher.cia_max_keysize;

if (nla_put(skb, CRYPTOCFGA_REPORT_CIPHER,
sizeof(struct crypto_report_cipher), &rcipher))
goto nla_put_failure;
return 0;

nla_put_failure:
return -EMSGSIZE;
}
