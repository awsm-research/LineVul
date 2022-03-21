static int crypto_report_akcipher(struct sk_buff *skb, struct crypto_alg *alg)
{
struct crypto_report_akcipher rakcipher;

	strlcpy(rakcipher.type, "akcipher", sizeof(rakcipher.type));

if (nla_put(skb, CRYPTOCFGA_REPORT_AKCIPHER,
sizeof(struct crypto_report_akcipher), &rakcipher))
goto nla_put_failure;
return 0;

nla_put_failure:
return -EMSGSIZE;
}
